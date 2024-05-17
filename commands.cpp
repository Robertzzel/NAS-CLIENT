#include "commands.h"

Command::Command(QString host, int port): QObject(0) {
    this->host = host;
    this->port = port;
}

QString Command::Login(QString username, QString password, bool& success) {
    MessageHandler socket;
    if(!socket.Connect(this->host, this->port)){
        emit statusSet(true, "Cannot connect.");
        return "cannot connect to the server";
    }

    QByteArray rawMessage = this->LoginCommandNumber + (username + '\n' + password).toUtf8();
    if(!socket.Write(rawMessage)){
        return "cannot write to the server";
    }

    rawMessage = socket.Read();
    if(rawMessage.size() < 1){
        return "cannot read from the server";
    }

    success = rawMessage[0] == '\x00';
    if(!success){
        return "";
    }

    this->username = username;
    this->password = password;
    return "";
}

void Command::Upload(QString serverFileParentPath, QString fullLocalFilePath){
    QThread* thread = new QThread;
    UploadWorker* worker = new UploadWorker(serverFileParentPath, fullLocalFilePath, this);
    worker->moveToThread(thread);
    //connect(worker, &UploadWorker::error, this, &QThread::errorString);
    connect(thread, &QThread::started, worker, &UploadWorker::process);
    connect(worker, &UploadWorker::finished, thread, &QThread::quit);
    connect(worker, &UploadWorker::finished, worker, &UploadWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(worker, &UploadWorker::statusSet, this, &Command::setStatus);
    thread->start();
}

void Command::Download(QString fullServerFileName, QString fullClientFileName){
    QThread* thread = new QThread;
    DownloadWorker* worker = new DownloadWorker(fullServerFileName, fullClientFileName, this);
    worker->moveToThread(thread);
    //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, &QThread::started, worker, &DownloadWorker::process);
    connect(worker, &DownloadWorker::finished, thread, &QThread::quit);
    connect(worker, &DownloadWorker::finished, worker, &DownloadWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(worker, &DownloadWorker::statusSet, this, &Command::setStatus);
    thread->start();
}

QString Command::CreateDirectory(QString fullDirectoryName, bool& success){
    MessageHandler socket;
    if(!socket.Connect(this->host, this->port)){
        emit statusSet(true, "Cannot connect.");
        return "cannot connect to server";
    }

    QByteArray message = this->CreateDirectoryCommandNumber + (username + '\n' + password + '\n' + fullDirectoryName).toUtf8();
    if(!socket.Write(message)) {
        return "cannort write the server";
    }

    message = socket.Read();
    success = message.size() > 0 && message[0] == '\x00';
    return "";
}

QString Command::Remove(QString fullFileOrDirectoryName, bool& success){
    MessageHandler socket;
    if(!socket.Connect(this->host, this->port)){
        emit statusSet(true, "Cannot connect.");
        return "cannot connect to the server";
    }

    QByteArray message = this->RemoveCommandNumber + (username + '\n' + password + '\n' + fullFileOrDirectoryName).toUtf8();
    if(!socket.Write(message)) {
        return "cannot write to the server";
    }

    message = socket.Read();
    success = message.size() > 0 && message[0] == '\x00';
    return "";
}

QString Command::Rename(QString oldFullFilepath, QString newFullFilepath, bool& success){
    MessageHandler socket;
    if(!socket.Connect(this->host, this->port)){
        emit statusSet(true, "Cannot connect.");
        return "cannot connect to the server";
    }

    QByteArray message = this->RenameCommandNumber + (username + '\n' + password + '\n' + oldFullFilepath + '\n' + newFullFilepath).toUtf8();
    if(!socket.Write(message)) {
        return "cannot write to the server";
    }

    message = socket.Read();
    success = message.size() > 0 && message[0] == '\x00';
    return "";
}

QString Command::List(QString pathToList, QList<File>& files){
    MessageHandler socket;
    if(!socket.Connect(this->host, this->port)){
        emit statusSet(true, "Cannot connect.");
        return "cannot connect to the server";
    }

    QByteArray message = this->ListCommandNumber + (username + '\n' + password + '\n' + pathToList).toUtf8();
    if(!socket.Write(message)){
        return "cannot write the command message";
    }

    message = socket.Read();
    if(message.size() < 1){
        return "cannot read the response from the server";
    }
    if(message[0] != '\x00'){
        return "command failed";
    }
    message = message.remove(0, 1);

    QString stringMessage = QString::fromUtf8(message);
    if(stringMessage == ""){
        return "";
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

    return "";
}

QString Command::Info(QString& info){
    MessageHandler socket;
    if(!socket.Connect(this->host, this->port)){
        emit statusSet(true, "Cannot connect.");
        return "Cannot connect to server";
    }

    auto msg = this->InfoCommandNumber + (username + '\n' + password).toUtf8();
    if(!socket.Write(msg)){
        info = "";
        return "cannot write to the server";
    }

    msg = socket.Read();
    if(msg.size() > 0 && msg[0] != '\x00') {
        info = "";
        return "cannot read from the server";
    }

    msg = msg.remove(0, 1);
    return QString::fromUtf8(msg);
}

void Command::setStatus(bool status, QString msg){
    emit statusSet(status, msg);
}
