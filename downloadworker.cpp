#include "commands.h"

DownloadWorker::DownloadWorker(QString fullServerFileName, QString fullClientFileName, Command* cmd): QObject{nullptr} {
    this->fullClientFileName = fullClientFileName;
    this->fullServerFileName = fullServerFileName;
    this->cmd = cmd;
}

DownloadWorker::~DownloadWorker() {}

void DownloadWorker::process() {
    MessageHandler socket;

    connect(&socket, &MessageHandler::statusSet, this, &DownloadWorker::setStatus);

    if(!socket.Connect(this->cmd->host, this->cmd->port)){
        emit statusSet(true, "Cannot connect.");
        return;
    }

    QByteArray message = (Command::DownloadCommandNumber +
                          this->cmd->username +
                          '\n' +
                          this->cmd->password +
                          '\n' +
                          fullServerFileName).toUtf8();

    if(!socket.Write(message)){
        emit statusSet(true, "Cannot write to the server.");
        return;
    }

    auto msg = socket.Read();
    if(msg.size() > 0 && msg[0] != '\x00') {
        emit statusSet(true, "Cannot read from the server.");
        return;
    }

    QFile file(fullClientFileName);
    if(file.exists()){
        emit statusSet(true, "File already exists");
        return;
    }

    if(!file.open(QIODevice::WriteOnly)) {
        emit statusSet(true, "Cannot open the file.");
        return;
    }

    emit statusSet(true, "Download stared...");
    if(!socket.ReadFile(file)){
        emit statusSet(true, "Error while downloading");
        return;
    }

    emit statusSet(true, "File downloaded");
}

void DownloadWorker::setStatus(quint64 bytes){
    emit statusSet(true, "Downloaded " + QString::number(bytes) + " bytes");
}
