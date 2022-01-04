#include "tcpinterface.h"
#include <QDebug>

RemoteControlSocket::RemoteControlSocket(uint16_t port, MainWindow * win) : server(), _win(win){
	server.listen(QHostAddress::Any, port);
	connect(&server, &QTcpServer::newConnection, this, &RemoteControlSocket::addClient);
}

void RemoteControlSocket::addClient() {
	auto *client = server.nextPendingConnection();
	clients.push_back(client);
	connect(client, &QTcpSocket::readyRead, this, [this, client]() {
		while(client->canReadLine())
			this->handleLine(client->readLine().trimmed(), client);
	});
}

void RemoteControlSocket::handleLine(QString s, QTcpSocket *sock) {
    qInfo() << s;
    QStringList matches = s.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QString command = matches[0];
    matches.pop_front();
    QString args = s.remove(command);

    if (command == "start") {
        emit start();
        sock->write("OK");
    } else if (command == "stop") {
        emit stop();
        sock->write("OK");
    } else if (command == "update") {
        emit refresh_streams();
        sock->write("OK");
    } else if (command == "filename") {
        emit filename(args);
        sock->write("OK");
    } else if (command == "select") {
        if (matches[0] == "all") {
            if (matches.length() > 1)
                qInfo() << "Cannot parse command!";
            else
                emit select_all();
        } else if (matches[0] == "none" and matches.length() == 0) {
            if (matches.length() > 1)
                qInfo() << "Cannot parse command!";
            else
                emit select_none();
        } else {
            for( QString match : matches ) {
                emit select(match);
            }
        }
        sock->write("OK");
    } else if (command == "list") {
        sock->write(_win->getKnownStreams().toJsonString().c_str());
    } else if ( command == "deselect" ) {
        if (matches[0] == "all") {
            if (matches.length() > 1)
                qInfo() << "Cannot parse command!";
            else
                emit select_none();
        } else {
            for (QString match: matches) {
                emit deselect(match);
            }
        }
    }

    // TODO: select /deselect streams
	// TODO: send acknowledgement
	// TODO: get current state
	//
	// else this->sender()->sender("Whoops");
}
