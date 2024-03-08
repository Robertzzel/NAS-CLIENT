#ifndef COMMANDS_H
#define COMMANDS_H

#include<QByteArray>
#include"messagehandler.h"
#include<qfile.h>
#include<QFileInfo>
#include"file.h"

class Command
{
private:
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
    MessageHandler socket;

    Command(){}

    bool resetConnection(){
        if(!this->socket.Disconnect()){
            return false;
        }
        if(!this->socket.Connect(host, port)){
            return false;
        }
        return this->Login(this->username, this->password);
    }


public:

    static Command* GetCommand(QString host, int port, QString &error){
        Command* command = new Command;
        if(!command->socket.Connect(host, port)) {
            error = command->error();
            return nullptr;
        }
        command->host = host;
        command->port = port;
        return command;
    }

    QString error() {
        return this->socket.error();
    }

    bool Login(QString username, QString password) {
        QByteArray rawMessage = this->LoginCommandNumber + (username + '\n' + password).toUtf8();

        if(!this->socket.Write(rawMessage)){
            return false;
        }

        rawMessage = this->socket.Read();
        if(rawMessage.size() < 1){
            return false;
        }

        this->username = username;
        this->password = password;

        return rawMessage[0] == '\x00';
    }

    bool Upload(QString filePrefixPath, QFile& file){
        QByteArray message = this->UploadCommandNumber +
            (filePrefixPath + QFileInfo(file).fileName() +
            '\n' + QString::number(file.size())).toUtf8();

        if(!socket.Write(message)){
            return false;
        }

        QByteArray msg = socket.Read();
        if(msg.size() > 0 && msg[0] != '\x00') {
            return false;
        }

        if(!socket.WriteFile(file)){
            return false;
        }

        msg = socket.Read();
        return msg.size() > 0 && msg[0] == '\x00';
    }

    bool Download(QString fullFileName, QFile& fileToWrite){
        QByteArray message = this->DownloadCommandNumber + fullFileName.toUtf8();
        if(!socket.Write(message)){
            return false;
        }

        QByteArray msg = socket.Read();
        if(msg.size() > 0 && msg[0] != '\x00') {
            return false;
        }

        if(!socket.ReadFile(fileToWrite)){
            return false;
        }

        return this->resetConnection();
    }

    bool CreateDirectory(QString fullDirectoryName){
        QByteArray message = this->CreateDirectoryCommandNumber + fullDirectoryName.toUtf8();
        if(!socket.Write(message)) {
            return false;
        }

        message = socket.Read();
        return message.size() > 0 && message[0] != '\x00';
    }

    bool Remove(QString fullFileOrDirectoryName){
        QByteArray message = this->RemoveCommandNumber + fullFileOrDirectoryName.toUtf8();
        if(!socket.Write(message)) {
            return false;
        }

        message = socket.Read();
        return message.size() > 0 && message[0] != '\x00';
    }

    bool Rename(QString oldFullFilepath, QString newFullFilepath){
        QByteArray message = this->RenameCommandNumber + (oldFullFilepath + '\n' + newFullFilepath).toUtf8();
        if(!socket.Write(message)) {
            return false;
        }

        message = socket.Read();
        return message.size() > 0 && message[0] != '\x00';
    }

    bool List(QString pathToList, QList<File>& files){
        QByteArray message = this->ListCommandNumber + pathToList.toUtf8();
        if(!this->socket.Write(message)){
            return false;
        }

        message = socket.Read();
        if(message.size() < 1 || message[0] != '\x00'){
            return false;
        }
        message = message.remove(0, 1);

        QString stringMessage = QString::fromUtf8(message);
        if(stringMessage == ""){
            return true;
        }

        QStringList messageFiles = stringMessage.split('\x1c');
        for(int i = 0; i < messageFiles.length(); ++i) {
            QStringList fileAndDetails = messageFiles[i].split('\n');
            File file;
            file.name = fileAndDetails[0];
            file.size = fileAndDetails[1].toULongLong();
            file.isDir = fileAndDetails[2] == "true";
            file.type = fileAndDetails[3];
            file.created = fileAndDetails[4].toULongLong();
            files.append(file);
        }

        return true;
    }

    QString Info(){
        auto msg = this->InfoCommandNumber + QString("").toUtf8();
        if(!this->socket.Write(msg)){
            return "";
        }

        msg = socket.Read();
        if(msg.size() > 0 && msg[0] != '\x00') {
            return "";
        }

        msg = msg.remove(0, 1);
        return QString::fromUtf8(msg);
    }
};

#endif
