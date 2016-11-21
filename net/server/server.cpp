#include "server.h"

namespace sim
{
    namespace net
    {



server::server(const std::string &name, unsigned int port)
    : m_beacon(name, port)
{

}

server::~server()
{

}


    } // namespace net
} // namespace sim
