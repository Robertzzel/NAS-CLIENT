#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QProgressBar>
#include <QSslSocket>

class MessageHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageHandler();
    ~MessageHandler();
    bool Connect(QString, int);
    QByteArray Read();
    bool Write(QByteArray);
    bool ReadFile(QFile&);
    bool WriteFile(QFile&);
    bool Disconnect();
    QString error();
private:
    QSslSocket socket;
    bool waitForNBytes(quint64, int msecs = 3000);
    void close();
signals:
    void statusSet(quint64);
};

#endif // MESSAGEHANDLER_H
