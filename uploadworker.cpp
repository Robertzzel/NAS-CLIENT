#include "commands.h"
#include <QString>

UploadWorker::UploadWorker(QString serverFileParentPath, QString fullLocalFilePath, Command* cmd): QObject{nullptr} {
    this->serverFileParentPath = serverFileParentPath;
    this->fullLocalFilePath = fullLocalFilePath;
    this->cmd = cmd;
}

UploadWorker::~UploadWorker() {}

void UploadWorker::process() {
    MessageHandler socket;

    connect(&socket, &MessageHandler::statusSet, this, &UploadWorker::setStatus);

    if(!socket.Connect(this->cmd->host, this->cmd->port)){
        emit statusSet(true, "Cannot connect.");
        return;
    }

    QFile file(fullLocalFilePath);
    if(!file.exists()){
        emit statusSet(true, "The selected file does not exists.");
        return;
    }

    if(!file.open(QIODevice::ReadOnly)) {
        emit statusSet(true, "Open the file for reading.");
        return;
    }

    QByteArray message = (Command::UploadCommandNumber +
                          this->cmd->username +
                          '\n' +
                          this->cmd->password +
                          '\n' +
                          serverFileParentPath + QFileInfo(file).fileName() +
                          '\n' +
                          QString::number(file.size())).toUtf8();

    if(!socket.Write(message)){
        emit statusSet(true, "Cannot write to the server.");
        return;
    }

    message = socket.Read();
    if(message.size() < 1){
        emit statusSet(true, "Cannot read from the server.");
        return;
    }
    if(message[0] != '\x00') {
        emit statusSet(true, QString::fromUtf8(message.remove(0, 1)));
        return;
    }

    emit statusSet(true, "Upload started...");
    if(!socket.WriteFile(file)){
        emit statusSet(true, "Cannot write to the file.");
        return;
    }

    // TODO repune codul asta, e scos doar pentru proiect CC
    // message = socket.Read();
    // if(message.size() > 0 && message[0] == '\x00'){
    //     emit statusSet(true, "Upload successfull.");
    // } else {
    //     emit statusSet(true, "Upload failed." + socket.error());
    // }
}

void UploadWorker::setStatus(quint64 bytes){
    emit statusSet(true, "Uploaded " + QString::number(bytes) + " bytes");
}
