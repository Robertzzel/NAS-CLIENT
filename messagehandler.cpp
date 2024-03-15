#include "messagehandler.h"
#include <qssl.h>

#define BUFFER_SIZE 1048576

MessageHandler::MessageHandler() : QObject{0} {}

MessageHandler::~MessageHandler() {}

bool MessageHandler::Connect(QString host, int port) {
    socket.connectToHostEncrypted(host, port);
    socket.ignoreSslErrors();
    return socket.waitForEncrypted(-1);
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

    quint64 bytesUploaded = 0;
    this->waitForNBytes(BUFFER_SIZE, -1);
    while (this->socket.bytesAvailable()) {
        quint64 msgSize = this->socket.read(buffer, BUFFER_SIZE);
        if(msgSize == 0){
            break;
        }
        file.write(buffer, msgSize);
        this->waitForNBytes(BUFFER_SIZE, -1);
        bytesUploaded += msgSize;
        emit statusSet(bytesUploaded);
    } //vezi daca arata bine pe telefon ca UI

    return this->socket.error() == QAbstractSocket::RemoteHostClosedError;
}

bool MessageHandler::WriteFile(QFile &file) {
    QDataStream out(&socket);
    quint64 fileSize = static_cast<quint64>(file.size());
    out << fileSize;

    quint64 bytesDownloaded = 0;
    char buffer[BUFFER_SIZE];
    while (!file.atEnd()) {
        quint64 bytesRead = file.read(buffer, BUFFER_SIZE);
        if(bytesRead == 0){
            break;
        }
        bytesDownloaded += bytesRead;
        emit statusSet(bytesDownloaded);
        this->socket.write(buffer, bytesRead);
        if(!socket.waitForBytesWritten(-1)){
            return false;
        }
    }

    return true;
}

bool MessageHandler::waitForNBytes(quint64 size, int msecs = 3000) {
    return static_cast<quint64>(this->socket.bytesAvailable()) < size ? this->socket.waitForReadyRead(msecs) : true;
}

void MessageHandler::close() {
    this->socket.close();
}

QString MessageHandler::error() {
    return this->socket.errorString();
}

bool MessageHandler::Disconnect(){
    if(socket.state() != QAbstractSocket::ConnectedState) {
        return true;
    }

    socket.disconnectFromHost();
    return socket.waitForDisconnected(-1);
}
