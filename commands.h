#ifndef COMMANDS_H
#define COMMANDS_H

#include<QByteArray>
#include"messagehandler.h"
#include<qfile.h>
#include<QFileInfo>
#include"file.h"
#include<QThreadPool>

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

    static Command* GetCommand(QString host, int port, QString &error);
    QString error();
    bool Login(QString username, QString password);
    void Upload(QString serverFileParentPath, QString fullLocalFilePath);
    void Download(QString fullServerFileName, QString fullClientFileName);
    bool CreateDirectory(QString fullDirectoryName);
    bool Remove(QString fullFileOrDirectoryName);
    bool Rename(QString oldFullFilepath, QString newFullFilepath);
    bool List(QString pathToList, QList<File>& files);
    QString Info();
signals:
    void statusSet(bool enabled, QString message);
private:
    MessageHandler socket;
    QThreadPool threadPool;
    Command();
    bool resetConnection();

    void setStatus(bool enabled, QString message);
};

#endif
