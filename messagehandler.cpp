#include "messagehandler.h"

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
    quint64 bufferSize = 4096;
    char buffer[bufferSize];
    this->waitForNBytes(bufferSize, -1);
    while (this->socket.bytesAvailable()) {
        quint64 msgSize = this->socket.read(buffer, bufferSize);
        if(msgSize == 0){
            break;
        }

        file.write(buffer, msgSize);
        this->waitForNBytes(bufferSize, -1);
    }

    return this->socket.error() == QAbstractSocket::RemoteHostClosedError;
}

bool MessageHandler::WriteFile(QFile &file) {
    QDataStream out(&socket);
    quint64 fileSize = static_cast<quint64>(file.size());
    out << fileSize;

    while (!file.atEnd()) {
        QByteArray buffer = file.readAll();
        file.write(buffer);
    }

    return socket.waitForBytesWritten();
}

bool MessageHandler::waitForNBytes(quint64 size, int msecs = 3000) {
    return static_cast<quint64>(this->socket.bytesAvailable()) < size ? this->socket.waitForReadyRead(msecs) : true;
}

void MessageHandler::close() {
    this->socket.close();
    this->socket.waitForDisconnected(-1);
}

bool MessageHandler::ResetConnection(QString host, int port) {
    if(socket.state() == QAbstractSocket::ConnectedState) {
        socket.disconnect();
        socket.waitForDisconnected(-1);
    }
    socket.connectToHost(host, port);
    return socket.waitForConnected(-1);
}
