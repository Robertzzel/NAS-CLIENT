#ifndef COMMANDS_H
#define COMMANDS_H

#include<QByteArray>
#include"messagehandler.h"
#include<qfile.h>
#include<QFileInfo>
#include"file.h"
#include<QThreadPool>

class Command;

class DownloadWorker : public QObject {
    Q_OBJECT
public:
    DownloadWorker(QString, QString, Command*);
    ~DownloadWorker();
    void process();
    void setStatus(quint64);
signals:
    void finished();
    void error(QString err);
    void statusSet(bool, QString);
private:
    QString fullServerFileName, fullClientFileName;
    Command* cmd;
};

class UploadWorker : public QObject{
    Q_OBJECT
public:
    UploadWorker(QString, QString, Command*);
    ~UploadWorker();
    void process();
    void setStatus(quint64);
signals:
    void finished();
    void error(QString err);
    void statusSet(bool enabled, QString message);
private:
    QString serverFileParentPath, fullLocalFilePath;
    Command* cmd;
};

class Command: public QObject
{
    Q_OBJECT
public:
    static inline char UploadCommandNumber          = '\x00';
    static inline char DownloadCommandNumber        = '\x01';
    static inline char CreateDirectoryCommandNumber = '\x02';
    static inline char RemoveCommandNumber          = '\x03';
    static inline char RenameCommandNumber          = '\x04';
    static inline char LoginCommandNumber           = '\x05';
    static inline char ListCommandNumber            = '\x06';
    static inline char InfoCommandNumber            = '\x07';
    QString host, username, password;
    int port;

    Command(QString host, int port);
    QString error();
    QString Login(QString username, QString password, bool& success);
    void Upload(QString serverFileParentPath, QString fullLocalFilePath);
    void Download(QString fullServerFileName, QString fullClientFileName);
    QString CreateDirectory(QString fullDirectoryName, bool& success);
    QString Remove(QString fullFileOrDirectoryName, bool& success);
    QString Rename(QString oldFullFilepath, QString newFullFilepath, bool& success);
    QString List(QString pathToList, QList<File>& files);
    QString Info(QString& info);
signals:
    void statusSet(bool enabled, QString message);
private:
    QThreadPool threadPool;
    Command();
    bool resetConnection();
    void setStatus(bool enabled, QString message);
};

#endif
