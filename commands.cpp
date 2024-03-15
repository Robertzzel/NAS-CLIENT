#include "commands.h"

Command::Command(): QObject(0) {

}

Command* Command::GetCommand(QString host, int port, QString &error){
    Command* command = new Command;
    if(!command->socket.Connect(host, port)) {
        error = command->error();
        return nullptr;
    }
    command->host = host;
    command->port = port;
    return command;
}

bool Command::Login(QString username, QString password) {
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

void Command::Upload(QString serverFileParentPath, QString fullLocalFilePath){
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
        serverFileParentPath + QFileInfo(file).fileName() +
        '\n' +
        QString::number(file.size())).toUtf8();

    if(!socket.Write(message)){
        emit statusSet(true, "Cannot write to the server.");
        return;
    }

    message = socket.Read();
    if(message.size() > 0 && message[0] != '\x00') {
        emit statusSet(true, "Cannot read from the server.");
        return;
    }

    emit statusSet(true, "Upload started...");
    if(!socket.WriteFile(file)){
        emit statusSet(true, "Cannot write to the file.");
        return;
    }

    message = socket.Read();
    if(message.size() > 0 && message[0] == '\x00'){
        emit statusSet(true, "Upload successfull.");
    } else {
        emit statusSet(true, "Upload failed.");
    }
}

void Command::Download(QString fullServerFileName, QString fullClientFileName){ 
    QByteArray message = this->DownloadCommandNumber +
                         fullServerFileName.toUtf8();

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

    this->resetConnection();
    emit statusSet(true, "File downloaded");
}

bool Command::CreateDirectory(QString fullDirectoryName){
    QByteArray message = this->CreateDirectoryCommandNumber + fullDirectoryName.toUtf8();
    if(!socket.Write(message)) {
        return false;
    }

    message = socket.Read();
    return message.size() > 0 && message[0] != '\x00';
}

bool Command::Remove(QString fullFileOrDirectoryName){
    QByteArray message = this->RemoveCommandNumber + fullFileOrDirectoryName.toUtf8();
    if(!socket.Write(message)) {
        return false;
    }

    message = socket.Read();
    return message.size() > 0 && message[0] != '\x00';
}

bool Command::Rename(QString oldFullFilepath, QString newFullFilepath){
    QByteArray message = this->RenameCommandNumber + (oldFullFilepath + '\n' + newFullFilepath).toUtf8();
    if(!socket.Write(message)) {
        return false;
    }

    message = socket.Read();
    return message.size() > 0 && message[0] != '\x00';
}

bool Command::List(QString pathToList, QList<File>& files){
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

QString Command::Info(){
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

bool Command::resetConnection(){
    if(!this->socket.Disconnect()){
        return false;
    }
    if(!this->socket.Connect(host, port)){
        return false;
    }
    return this->Login(this->username, this->password);
}

QString Command::error() {
    return this->socket.error();
}

void Command::setStatus(bool status, QString msg){
    emit statusSet(status, msg);
}
