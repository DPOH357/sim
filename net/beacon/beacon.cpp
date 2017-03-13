#include "beacon.h"

#include "beaconmode.h"
#include <log/log.h>

namespace sim
{
    namespace net
    {

using namespace sim::base;

beacon::beacon(const std::string& name, unsigned int port)
    : m_mode(nullptr)
    , m_id(0)
{
    strncpy(m_name, name.c_str(), beacon::beacon_name_max_length);
    m_name[beacon::beacon_name_max_length - 1] = 0;

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
                net::beacons_list beacons_list;
                authentication_mode->get_beacons_list(beacons_list);
                delete m_mode;
                beacon_message_data beacon_data(m_id, m_name);
                m_mode = new net::beacon_mode_default(m_broadcast, beacon_data, beacons_list);
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

void beacon::get_beacons_list(net::beacons_list &beacons_list) const
{
    auto mode_default =
            dynamic_cast<const net::beacon_mode_default*>(m_mode);

    if(mode_default)
    {
        mode_default->get_beacons_list(beacons_list);
    }
}


    } // namespace net
} // namespace sim
