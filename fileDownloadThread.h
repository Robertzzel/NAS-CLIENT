#ifndef FILEDOWNLOADTHREAD_H
#define FILEDOWNLOADTHREAD_H

#include<QRunnable>
#include"messagehandler.h"
#include<qfile.h>

class FileDownloadTask: public QObject, public QRunnable {
    Q_OBJECT
public:
    FileDownloadTask(const QString &host, int port, const QString &fileName, QByteArray& message): QObject(0) {
        this->host = host;
        this->port = port;
        this->fileName = fileName;
        this->message = message;
    }

    void run() override {
        MessageHandler downloadConnection;
        if(!downloadConnection.Connect(this->host, this->port)){
            emit result(false);
            return;
        }

        if(!downloadConnection.Write(message)){
            emit result(false);
            return;
        }

        auto msg = downloadConnection.Read();
        if(msg.size() > 0 && msg[0] != '\x00') {
            emit result(false);
            return;
        }

        QFile file(this->fileName);
        if(file.exists()){
            emit result(false);
            return;
        }

        if(!file.open(QIODevice::WriteOnly)) {
            emit result(false);
            return;
        }

        if(!downloadConnection.ReadFile(file)){
            emit result(false);
            return;
        }

        downloadConnection.Disconnect();
        emit result(true);
    }
private:
    QString fileName;
    QString host;
    int port;
    QByteArray message;

signals:
    void result(bool);
};

#endif // FILEDOWNLOADTHREAD_H
