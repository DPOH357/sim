#ifndef USERLISTDETECOR_H
#define USERLISTDETECOR_H

#include "chatagent.h"
#include <net/broadcast.h>
#include <QHash>
#include <QTime>

class UserListAgent : public ChatAgent
{
    Q_OBJECT

public:
    UserListAgent(unsigned int port, QString name);

    void run() override;

    boost::asio::ip::udp::endpoint getUserEndpoint(QString name);

signals:
    void send_userIn(QString name);
    void send_userOut(QString name);

private:
    struct UserData
    {
        QTime timeCheck;
        boost::asio::ip::udp::endpoint endpoint;

        UserData(QTime _timeCheck, boost::asio::ip::udp::endpoint _endpoint)
            : timeCheck(_timeCheck)
            , endpoint(_endpoint)
        {

        }
    };

    const int      m_dTimeCheck; // msec
    const int      m_dTimeUserLeft; // msec
    enum _ { name_length = 32 };
    QString m_name;

    sim::net::broadcast_ptr m_broadcast;
    QTime   m_timePrev;

    QHash<QString, UserData> m_usersList;
};

#endif // USERLISTDETECOR_H
