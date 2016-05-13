#ifndef USERDSIALOGAGENT_H
#define USERDSIALOGAGENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>

#include <net/tcp_sender.h>
#include <net/tcp_receiver.h>

class UserDialog : public QObject
{
    Q_OBJECT

public:
    UserDialog(QString myName, unsigned short port);

    void run();

    void sendMessage(QString addressStr, QString message);

signals:
    void send_messageReceived(QString addressStr, QString message);

private:
    struct Message
    {
        enum _ { Size = 256 };
        char text[Size];

        Message()
        {
            std::memset(text, 0, Size);
        }

        Message(QString str)
        {
            std::strncpy(text, str.toLocal8Bit().constData(), Size);
        }
    };

private:
    const unsigned short m_port;

    sim::net::tcp_receiver_shptr    m_receiver;
    sim::net::tcp_sender_shptr      m_sender;
};

#endif // USERDSIALOGAGENT_H
