#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QCloseEvent>
#include <QComboBox>
#include <QListWidget>
#include <QMainWindow>
#include <QStringList>
#include <QTimer>
#include <memory> //for std::unique_ptr

// LSL
#include <lsl_cpp.h>

namespace Ui {
class MainWindow;
}

class recording;
class RemoteControlSocket;

class StreamItem {
	
public:
	StreamItem(std::string stream_name, std::string stream_type, std::string source_id,
		std::string hostname, bool required)
		: name(stream_name), type(stream_type), id(source_id), host(hostname), checked(required) {}
	
	QString listName() { return QString::fromStdString(name + " (" + host + ")"); }
	std::string name;
	std::string type;
	std::string id;
	std::string host;
    std::string toJsonString() const {
        return std::string(R"({"name":")" + name +R"(","type":")" + type + R"(","id":")" + id + R"(","host":")" + host + "\"}");
    }
	bool checked;
};


class StreamList: public QList<StreamItem> {
public:
    std::string toJsonString() const {
        std::string json("[");
        for(int i = 0; i < this->length(); ++i) {
            json.append(this->at(i).toJsonString());
            if(i < this->length() - 1) {
                json.append(",");
            }
        }
        json.append("]");
        return json;
    }
};

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent, const char *config_file);
	~MainWindow() noexcept override;

    StreamList getKnownStreams() {
        return knownStreams;
    }

private slots:
	void statusUpdate(void) const;
	void closeEvent(QCloseEvent *ev) override;
	void blockSelected(const QString &block);
	std::vector<lsl::stream_info> refreshStreams(void);
	void startRecording(void);
	void stopRecording(void);
	void selectAllStreams();
	void selectNoStreams();
	void buildFilename();
	void buildBidsTemplate();
	void printReplacedFilename();
	void enableRcs(bool bEnable);
	void rcsCheckBoxChanged(bool checked);
	void rcsUpdateFilename(QString s);
	void rcsStartRecording();
    void rcsSelectStream(QString s);
    void rcsDeselectStream(QString s);
    void rcsportValueChangedInt(int value);


private:
	QString replaceFilename(QString fullfile) const;
	// function for loading / saving the config file
	QString find_config_file(const char *filename);
	QString counterPlaceholder() const;
	void load_config(QString filename);
	void save_config(QString filename);

	std::unique_ptr<recording> currentRecording;
	std::unique_ptr<RemoteControlSocket> rcs;

	int startTime;
	std::unique_ptr<QTimer> timer;

    StreamList knownStreams;
	QSet<QString> missingStreams;
	std::map<std::string, int> syncOptionsByStreamName;

	// QString recFilename;
	QString legacyTemplate;
	std::unique_ptr<Ui::MainWindow> ui; // window pointer

	// @Doug1983 added to suppress pop-ups when remotely starting recording
	// and missing streams or having some unchecked streams
	bool hideWarnings = false;
};

#endif // MAINWINDOW_H
