#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>

class MessageHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageHandler(QObject *parent = nullptr);

    bool Connect(QString, int);
    QByteArray Read();
    bool Write(QByteArray);
    bool Write(char* message, quint64 size);
    bool ReadFile(QFile&);
    bool WriteFile(QFile&);
private:
    QTcpSocket socket;
signals:
};

#endif // MESSAGEHANDLER_H
