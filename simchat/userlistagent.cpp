#include "userlistdetecor.h"

UserListDetecor::UserListDetecor(unsigned int port)
{
    m_broadcast = sim::net::broadcast::create(port);
}

void UserListDetecor::run()
{

}

