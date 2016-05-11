#include "beacon.h"

#include "beaconmode.h"
#include <base/tools.h>

namespace sim
{
    namespace beacon
    {

using namespace sim::base;

beacon::detector::detector(char name[], boost::asio::ip::address address, unsigned int port)
    : m_mode(nullptr)
    , m_id(0)
    , m_address(address)
{
    strcpy_s(m_name, beacon::data::name_size, name);

    const unsigned short broadcast_messages_buffer_size = 32;
    const unsigned short repeat_broadcast_message_count = 3;

    m_sender
        = net::broadcast_sender<sim::beacon::message>::create(
                port,
                new sim::net::gate_message<sim::beacon::message>(
                    broadcast_messages_buffer_size,
                    repeat_broadcast_message_count));
    m_receiver
        = net::broadcast_receiver<sim::beacon::message>::create(
                port,
                new sim::net::gate_message<sim::beacon::message>(
                    broadcast_messages_buffer_size));

    m_mode = new sim::beacon::mode_authen(m_sender, m_receiver);
}

beacon::detector::~detector()
{
    delete m_mode;
}

void beacon::detector::run()
{
    if(!m_mode->run())
    {
        auto authentication_mode = dynamic_cast<sim::beacon::mode_authen*>(m_mode);
        if(authentication_mode)
        {
            m_id = authentication_mode->get_free_id();
            if(m_id)
            {
                delete m_mode;
                beacon::data beacon_data(m_id, m_name, m_address);
                m_mode = new sim::beacon::mode_default(m_sender, m_receiver, beacon_data);
                log::message(log::level::Debug, "Start default mode");
            }
        }
        else
        {
            delete m_mode;
            m_mode = new sim::beacon::mode_authen(m_sender, m_receiver);
            log::message(log::level::Debug, "Start authentication mode");
        }
    }
}


    } // namespace beacon
} // namespace sim
