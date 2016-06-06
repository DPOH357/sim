#include "beaconmode.h"

#include <log/log.h>
#include <base/tools.h>

namespace sim
{
    namespace net
    {


net::beacon_mode_authen::beacon_mode_authen(boost::shared_ptr<net::broadcast> broadcast)
    : mc_timer_duration(500)
    , mc_timer_main_duration(3000)
    , m_broadcast(broadcast)
    , m_last_response_message_mark(0)
{
    m_timer.start(mc_timer_duration);
    m_timer_main.start(mc_timer_main_duration);
}

bool net::beacon_mode_authen::run()
{
    if(m_timer_main.is_cutoff())
    {
        return false;
    }

    if(m_timer.is_cutoff())
    {
        bool bRequest = false;
        net::beacon_message receiving_message;
        while(m_broadcast->get_message(receiving_message))
        {
            if(receiving_message.mark != m_last_response_message_mark)
            {
                switch(receiving_message.message_type)
                {
                case net::beacon_message_type::Request:
                    {
                        bRequest = true;
                        log::message(log::level::Debug, "Receive request message");
                    }
                    break;

                case net::beacon_message_type::Response:
                    {
                        m_beacons_list.insert( beacon_data_pair(receiving_message.data.id, receiving_message.data) );
                        log::message(log::level::Debug, std::string("Receive response message, busy id ") + std::to_string(receiving_message.data.id));
                    }
                    break;

                case net::beacon_message_type::Invalid:
                    log::message(log::level::Debug, "Receive invalid message");
                default:
                    break;
                }
            }
        }

        // Если кто-нибудь другой запрашивает, ждём
        if(bRequest)
        {
            m_timer_main.start(mc_timer_main_duration);
            log::message(log::level::Debug, "Timeout");
        }
        else
        {
            m_last_response_message_mark = (unsigned int)sim::base::random(1, 0xFFFFFFFF);
            net::beacon_message request_message(m_last_response_message_mark);
            m_broadcast->send_message(request_message);

            log::message(log::level::Debug, "Send request");
        }

        m_timer.start(base::random(0, mc_timer_duration));
    }

    return true;
}

unsigned int net::beacon_mode_authen::get_free_id() const
{
    unsigned int id = 1;

    while(id)
    {
        auto i = m_beacons_list.find(id);
        if(i == m_beacons_list.end())
        {
            return id;
        }
        id++;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

net::beacon_mode_default::beacon_mode_default(
        boost::shared_ptr<net::broadcast> broadcast,
        beacon_message_data beacon_data)
    : mc_time_duration(500)
    , m_broadcast(broadcast)
    , m_beacon_data(beacon_data)
    , m_last_response_message_mark(0)
{
    m_timer.start(mc_time_duration);
}

bool net::beacon_mode_default::run()
{
    if(m_timer.is_cutoff())
    {
        bool bRequest = false;

        net::beacon_message receive_message;
        while(m_broadcast->get_message(receive_message))
        {
            if(receive_message.mark != m_last_response_message_mark)
            {
                bRequest = true;
            }
        }

        if(bRequest)
        {
            m_last_response_message_mark = sim::base::random(1, 0xFFFFFFFF);
            net::beacon_message response_message(m_last_response_message_mark, m_beacon_data);
            m_broadcast->send_message(response_message);
            log::message(log::level::Debug, "Send response message");
        }

        m_timer.start(mc_time_duration);
    }
    return true;
}


    } //namespace net
} // namespace sim
