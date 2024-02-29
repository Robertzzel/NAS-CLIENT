#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QProgressBar>
#include "commands.h"

class MessageHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageHandler(QObject *parent = nullptr);
    ~MessageHandler();
    bool Connect(QString, int);
    QByteArray Read();
    bool Write(QByteArray);
    bool ReadFile(QFile&);
    bool WriteFile(QFile&);
    bool ResetConnection(QString, int);
    bool Login(QString, QString);
    bool WriteCommand(Commnad, QByteArray&);
    QByteArray WriteCommandAndRead(Commnad, QByteArray&);
private:
    QTcpSocket socket;
    bool waitForNBytes(quint64, int);
    void close();
signals:
};

#endif // MESSAGEHANDLER_H
