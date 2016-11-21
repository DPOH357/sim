#ifndef NETSERVER_H
#define NETSERVER_H

#include "net/beacon/beacon.h"
#include "base/timer.h"

namespace sim
{
    namespace net
    {


class server
{
public:
    server(const std::string& name, unsigned int port);
    ~server();

private:
    net::beacon     m_beacon;
};


    } // namespace net
} // namespace sim

#endif // NETSERVER_H
