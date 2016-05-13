#ifndef USERSLISTDETECOR_H
#define USERSLISTDETECOR_H

#include <QObject>
#include <QHash>
#include <QTime>

#include <net/broadcast.h>

class UsersListAgent : public QObject
{
    Q_OBJECT

public:
    UsersListAgent(unsigned int port, QString name);

    void run();

signals:
    void send_userIn(QString addressStr, QString userName);
    void send_userOut(QString addressStr, QString userName);

private:
    struct UserData
    {
        QTime timeCheck;
        QString name;
        QString addressStr;

        UserData(QTime _timeCheck, QString _addressStr, QString _name)
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

    QHash<QString, UserData> m_usersList; // by name
};

#endif // USERSLISTDETECOR_H
