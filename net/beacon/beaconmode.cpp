#include "beaconmode.h"

#include <log/log.h>
#include <base/tools.h>

namespace sim
{
    namespace net
    {


net::beacon_mode_authen::beacon_mode_authen(boost::shared_ptr<net::broadcast> broadcast)
    : mc_timer_request_duration(250)
    , mc_timer_life_duration(1000)
    , m_broadcast(broadcast)
    , m_timer_request(false)
    , m_timer_life(false)
    , m_last_response_message_mark(0)
{
    m_timer_request.start(mc_timer_request_duration);
    m_timer_life.start(mc_timer_life_duration);
}

bool net::beacon_mode_authen::run()
{
    if(m_timer_life.is_cutoff())
    {
        return false;
    }

    net::beacon_message receiving_message;
    std::string address_str_sender;
    unsigned short port_sender;
    while(m_broadcast->get_message(receiving_message, &address_str_sender, &port_sender))
    {
        if(receiving_message.mark != m_last_response_message_mark)
        {
            switch(receiving_message.message_type)
            {
            case net::beacon_message_type::Request:
                {
                    unsigned int timer_request_random = base::random(0, mc_timer_request_duration);

                    m_timer_life.start(
                                2*mc_timer_life_duration +
                                timer_request_random);

                    m_timer_request.start(
                                mc_timer_life_duration +
                                mc_timer_request_duration +
                                timer_request_random);

                    log::message(log::level::Debug, "Timeout");
                }
                break;

            case net::beacon_message_type::Response:
                {
                    net::beacon_data beacon_data(
                                receiving_message.data.id,
                                receiving_message.data.name,
                                address_str_sender,
                                port_sender);

                    m_beacons_list.insert(
                                beacon_data_pair(receiving_message.data.id,
                                                 beacon_data) );
                    log::message(
                                log::level::Debug,
                                std::string("Receive response message, busy id ")
                                    + std::to_string(receiving_message.data.id));
                }
                break;

            case net::beacon_message_type::Invalid:
                log::message(log::level::Debug, "Receive invalid message");
            default:
                break;
            }
        }
    }

    if(m_timer_request.is_cutoff())
    {
        m_last_response_message_mark = (unsigned int)sim::base::random(1, 0xFFFFFFFF);
        net::beacon_message request_message(m_last_response_message_mark);
        m_broadcast->send_message(request_message);

        m_timer_request.start(mc_timer_request_duration);

        log::message(log::level::Debug, "Send request");
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

void beacon_mode_authen::get_beacons_list(net::beacons_list &beacons_list) const
{
    beacons_list = m_beacons_list;
}

///////////////////////////////////////////////////////////////////////////////

net::beacon_mode_default::beacon_mode_default(
        boost::shared_ptr<net::broadcast> broadcast,
        beacon_message_data beacon_data, const sim::net::beacons_list &beacons_list)
    : mc_time_duration(1000)
    , m_broadcast(broadcast)
    , m_timer_responce(true)
    , m_beacon_data(beacon_data)
    , m_last_response_message_mark(0)
    , m_beacons_list(beacons_list)
{
    m_timer_responce.start(mc_time_duration);
}

bool net::beacon_mode_default::run()
{
    if(m_timer_responce.is_cutoff())
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
    }
    return true;
}

void beacon_mode_default::get_beacons_list(beacons_list &beacons_list) const
{
    beacons_list = m_beacons_list;
}


    } //namespace net
} // namespace sim
