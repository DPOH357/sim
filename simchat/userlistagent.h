#ifndef USERLISTDETECOR_H
#define USERLISTDETECOR_H

#include "chat_manager.h"
#include <net/broadcast.h>
#include <QHash>

class UserListDetecor : public ChatManager
{
    Q_OBJECT

private:
    struct Message
    {
        enum class Type
        {
            None
        };

        Type type;
    };

    struct UserData
    {
        char name[32];

    };

public:
    UserListDetecor(unsigned int port);

    void run() override;

private:
    sim::net::broadcast_ptr m_broadcast;
};

#endif // USERLISTDETECOR_H
