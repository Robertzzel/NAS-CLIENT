#include "messagehandler.h"

#define BUFFER_SIZE 4096

MessageHandler::MessageHandler(QObject *parent) : QObject{parent} {}

MessageHandler::~MessageHandler() {}

bool MessageHandler::Connect(QString host, int port) {
    socket.connectToHost(host, port);
    return socket.waitForConnected(-1);
}

QByteArray MessageHandler::Read() {
    QDataStream in(&socket);
    quint64 size;

    this->waitForNBytes(8, -1);
    in >> size;

    this->waitForNBytes(size, -1);
    return this->socket.read(size);
}

bool MessageHandler::Write(QByteArray message) {
    QDataStream out(&socket);
    out << static_cast<quint64>(message.size());
    this->socket.write(message);

    return socket.waitForBytesWritten(-1);
}

bool MessageHandler::ReadFile(QFile &file) {
    char buffer[BUFFER_SIZE];

    this->waitForNBytes(BUFFER_SIZE, -1);
    while (this->socket.bytesAvailable()) {
        quint64 msgSize = this->socket.read(buffer, BUFFER_SIZE);
        if(msgSize == 0){
            break;
        }

        file.write(buffer, msgSize);
        this->waitForNBytes(BUFFER_SIZE, -1);
    }

    return this->socket.error() == QAbstractSocket::RemoteHostClosedError;
}

bool MessageHandler::WriteFile(QFile &file) {
    QDataStream out(&socket);
    quint64 fileSize = static_cast<quint64>(file.size());
    out << fileSize;

    char buffer[BUFFER_SIZE];
    while (!file.atEnd()) {
        quint64 bytesRead = file.read(buffer, BUFFER_SIZE);
        if(bytesRead == 0){
            break;
        }
        this->socket.write(buffer, bytesRead);
    }

    return socket.waitForBytesWritten();
}

bool MessageHandler::waitForNBytes(quint64 size, int msecs = 3000) {
    return static_cast<quint64>(this->socket.bytesAvailable()) < size ? this->socket.waitForReadyRead(msecs) : true;
}

void MessageHandler::close() {
    this->socket.close();
}

bool MessageHandler::ResetConnection(QString host, int port) {
    if(socket.state() == QAbstractSocket::ConnectedState) {
        socket.disconnect();
        socket.waitForDisconnected(-1);
    }
    socket.connectToHost(host, port);
    return socket.waitForConnected(-1);
}

bool MessageHandler::Login(QString username, QString password){
    QByteArray rawMessage = (username + '\n' + password).toUtf8();
    rawMessage.prepend(Command::Login);
    this->Write(rawMessage);

    rawMessage = this->Read();
    if(rawMessage.size() < 1){
        return false;
    }
    return rawMessage[0] == '\x00';
}

bool MessageHandler::WriteCommand(Command cmd, QByteArray& msg) {
    msg.prepend(cmd);
    bool result = this->Write(msg);
    msg.removeAt(0);
    return result;
}

QByteArray MessageHandler::WriteCommandAndRead(Command cmd, QByteArray& msg) {
    if(!this->WriteCommand(cmd, msg)) {
        return QByteArray();
    }

    return this->Read();
}
