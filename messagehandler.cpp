#include "messagehandler.h"

MessageHandler::MessageHandler(QObject *parent)
    : QObject{parent}
{}


bool MessageHandler::Connect(QString host, int port) {
    socket.connectToHost(QHostAddress(host), port);
    return socket.waitForConnected(3000);
}

QByteArray MessageHandler::Read() {
    this->socket.waitForReadyRead();
    QDataStream in(&socket);
    quint64 size;
    in >> size;

    QByteArray message;
    if (size > 0)
    {
        message.resize(size);
        in.readRawData(message.data(), size);
    }

    return message;
}

bool MessageHandler::Write(QByteArray message) {
    QDataStream out(&socket);
    out << static_cast<quint64>(message.size());
    out.writeRawData(message.data(), message.size());

    return socket.waitForBytesWritten();
}

bool MessageHandler::Write(char* message, quint64 size) {
    QDataStream out(&socket);
    out << size;
    this->socket.write(message, size);
    return socket.waitForBytesWritten();
}

bool MessageHandler::ReadFile(QFile &file) {
    this->socket.waitForReadyRead();
    QDataStream in(&socket);
    quint64 size;
    in >> size;

    int chunkSize = 4096;
    char buffer[chunkSize];
    quint64 bytesRead = 0;
    while (bytesRead < size) {
        this->socket.waitForReadyRead();
        quint64 bytesReceived = this->socket.read(buffer, chunkSize);
        if(bytesReceived == 0){
            return false;
        }
        quint64 bytesWritten = file.write(buffer, bytesReceived);
        if(bytesReceived != bytesWritten){
            return false;
        }
        bytesRead += bytesReceived;
    }

    return true;
}

bool MessageHandler::WriteFile(QFile &file) {
    QDataStream out(&socket);
    quint64 fileSize = static_cast<quint64>(file.size());
    out << fileSize;

    int chunkSize = 4096;
    char buffer[chunkSize];
    while (!file.atEnd()) {
        quint64 bytesRead = file.read(buffer, chunkSize);
        quint64 bytesWritten = file.write(buffer, bytesRead);
        if(bytesRead != bytesWritten){
            return false;
        }
    }

    return socket.waitForBytesWritten();
}
