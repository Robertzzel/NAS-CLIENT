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
    ~MessageHandler();
    bool Connect(QString, int);
    QByteArray Read();
    bool Write(QByteArray);
    bool ReadFile(QFile&);
    bool WriteFile(QFile&);
    bool ResetConnection(QString, int);
private:
    QTcpSocket socket;
    bool waitForNBytes(quint64, int);
    void close();
signals:
};

#endif // MESSAGEHANDLER_H
