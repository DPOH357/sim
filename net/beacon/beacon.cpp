#include "beacon.h"

#include "beaconmode.h"
#include <log/log.h>

namespace sim
{
    namespace net
    {

using namespace sim::base;

beacon::beacon(const std::string& name, const std::string& address_str, unsigned int port)
    : m_mode(nullptr)
    , m_id(0)
{
    strncpy(m_name, name.c_str(), beacon::beacon_name_max_length);
    m_name[beacon::beacon_name_max_length - 1] = 0;

    strncpy(m_address_str, address_str.c_str(), 16);
    m_address_str[15] = 0;

    m_broadcast = net::broadcast::create(port);
    m_broadcast->enable_message_mode(100);

    m_mode = new net::beacon_mode_authen(m_broadcast);
}

beacon::~beacon()
{
    delete m_mode;
}

void beacon::run()
{
    if(!m_mode->run())
    {
        auto authentication_mode = dynamic_cast<net::beacon_mode_authen*>(m_mode);
        if(authentication_mode)
        {
            m_id = authentication_mode->get_free_id();
            if(m_id)
            {
                delete m_mode;
                beacon_message_data beacon_data(m_id, m_name, m_address_str);
                m_mode = new net::beacon_mode_default(m_broadcast, beacon_data);
                log::message(log::level::Debug, "NetBeacon: Start default mode");
            }
        }
        else
        {
            delete m_mode;
            m_mode = new net::beacon_mode_authen(m_broadcast);
            log::message(log::level::Debug, "NetBeacon: Start authentication mode");
        }
    }
}


    } // namespace net
} // namespace sim
