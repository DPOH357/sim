#include "simbeaconmodedefault.h"

#include "simtools.h"


sim::beacon::mode_default::mode_default(
        boost::shared_ptr<net::broadcast_sender<sim::beacon::message> > sender,
        boost::shared_ptr<net::broadcast_receiver<sim::beacon::message> > receiver, unsigned char id)
    : mc_time_duration(500)
    , m_sender(sender)
    , m_receiver(receiver)
    , m_id(id)
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
            sim::beacon::message send_message(m_last_response_message_mark,
                                              sim::beacon::message_type::Response, m_id);
            m_sender->send(send_message);
            LOG_MESSAGE("Send response message");
        }

        m_timer.start(mc_time_duration);
    }
    return true;
}
