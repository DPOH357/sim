#include "beaconmode.h"

#include"tools/tools.h"

sim::beacon::mode_authen::mode_authen(
        boost::shared_ptr<net::broadcast_sender<sim::beacon::message> > sender,
        boost::shared_ptr<net::broadcast_receiver<sim::beacon::message> > receiver)
    : mc_timer_duration(500)
    , mc_timer_main_duration(3000)
    , m_sender(sender)
    , m_receiver(receiver)
    , m_last_response_message_mark(0)
{
    m_timer.start(mc_timer_duration);
    m_timer_main.start(mc_timer_main_duration);
}

bool sim::beacon::mode_authen::run()
{
    if(m_timer_main.is_cutoff())
    {
        return false;
    }

    if(m_timer.is_cutoff())
    {
        bool bRequest = false;
        sim::beacon::message receiving_message;
        while(m_receiver->get_message(receiving_message))
        {
            if(receiving_message.mark != m_last_response_message_mark)
            {
                switch(receiving_message.message_type)
                {
                case sim::beacon::message_type::Request:
                    {
                        bRequest = true;
                        LOG_MESSAGE("Receive request message");
                    }
                    break;

                case sim::beacon::message_type::Response:
                    {
                        m_beacons_list.insert( beacon_data_pair(receiving_message.data.id, receiving_message.data) );
                        LOG_MESSAGE(std::string("Receive response message, busy id ") + std::to_string(receiving_message.data.id));
                    }
                    break;

                case sim::beacon::message_type::Invalid:
                    LOG_MESSAGE("Receive invalid message");
                default:
                    break;
                }
            }
        }

        if(bRequest)
        {
            m_timer_main.start(mc_timer_main_duration);
            LOG_MESSAGE("Timeout");
        }
        else
        {
            m_last_response_message_mark = (unsigned int)sim::tool::random(1, 0xFFFFFFFF);
            sim::beacon::message request_message(m_last_response_message_mark);
            m_sender->send(request_message);

            LOG_MESSAGE("Send request");
        }

        m_timer.start(mc_timer_duration);
    }

    return true;
}

unsigned char sim::beacon::mode_authen::get_free_id() const
{
    unsigned char id = 1;

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

sim::beacon::mode_default::mode_default(boost::shared_ptr<net::broadcast_sender<sim::beacon::message> > sender,
        boost::shared_ptr<net::broadcast_receiver<sim::beacon::message> > receiver,
        beacon::data beacon_data)
    : mc_time_duration(500)
    , m_sender(sender)
    , m_receiver(receiver)
    , m_beacon_data(beacon_data)
    , m_last_response_message_mark(0)
{
    m_timer.start(mc_time_duration);
}

bool sim::beacon::mode_default::run()
{
    if(m_timer.is_cutoff())
    {
        bool bRequest = false;

        sim::beacon::message receive_message;
        while(m_receiver->get_message(receive_message))
        {
            if(receive_message.mark != m_last_response_message_mark)
            {
                bRequest = true;
            }
        }

        if(bRequest)
        {
            m_last_response_message_mark = sim::tool::random(1, 0xFFFFFFFF);
            sim::beacon::message response_message(m_last_response_message_mark, m_beacon_data);
            m_sender->send(response_message);
            LOG_MESSAGE("Send response message");
        }

        m_timer.start(mc_time_duration);
    }
    return true;
}
