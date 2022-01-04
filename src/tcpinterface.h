#pragma once

#include <cstdint>

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QDataStream>
#include <qregularexpression.h>
#include <memory>
#include "mainwindow.h"

class RemoteControlSocket : public QObject {
	Q_OBJECT
	QTcpServer server;
	QList<QTcpSocket*> clients;
public:
	RemoteControlSocket(uint16_t port, MainWindow * win);

private:
    std::shared_ptr<MainWindow> _win;

signals:
	void refresh_streams();
	void start();
	void stop();
	void filename(QString s);
	void select_all();
	void select_none();
    void select(QString s);

public slots:
	void addClient();
	void handleLine(QString s, QTcpSocket* sock);
};
