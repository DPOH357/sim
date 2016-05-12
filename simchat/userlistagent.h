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

signals:
    void send_userIn(QString addressStr, QString userName);
    void send_userOut(QString addressStr, QString userName);

private:
    struct UserData
    {
        QTime timeCheck;
        QString name;
        QString addressStr;

        UserData(QTime _timeCheck, QString _name, QString _addressStr)
            : timeCheck(_timeCheck)
            , name(_name)
            , addressStr(_addressStr)
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
