#include "broadcast_sender.h"

#include <log/log.h>
#include <base/gate.hpp>

namespace sim
{
    namespace net
    {


using namespace sim::base;

net::broadcast_sender::broadcast_sender(unsigned short port)
    : m_b_valid(true)
    , m_socket( m_io_service, ip::udp::endpoint(ip::udp::v4(), port) )
    , m_endpoint( ip::address_v4::broadcast(), port )
    , m_gate_send( new base::gate_stream<base::raw_data>() )
{
    m_socket.set_option( ip::udp::socket::reuse_address(true) );
    m_socket.set_option( socket_base::broadcast(true) );
}

net::broadcast_sender::~broadcast_sender()
{
    m_b_valid = false;
    m_socket.close();

    m_thread->join();
    delete m_thread;

    delete m_gate_send;

    log::message(log::level::Debug, std::string("Broadcast sender closed."));
}

boost::shared_ptr<broadcast_sender> broadcast_sender::create(unsigned short port)
{
    auto broadcast_sender
        = boost::shared_ptr<net::broadcast_sender>(new net::broadcast_sender(port));

    broadcast_sender->m_thread = new boost::thread(&net::broadcast_sender::run, broadcast_sender.get());

    return broadcast_sender;
}

void broadcast_sender::enable_message_mode(unsigned int messages_queue_length)
{
    delete m_gate_send;

    m_gate_send = new base::gate_message<base::raw_data>
            (messages_queue_length);
}

void broadcast_sender::send_message(const base::raw_data &raw_data)
{
    m_gate_send->push(raw_data);
}

void net::broadcast_sender::run()
{
    do_send();
    m_io_service.run();
}

void net::broadcast_sender::do_send()
{
    log::message(log::level::Debug, std::string("UDP: Do send."));

    while(!m_gate_send->pop(m_buffer))
    {
        boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
    }

    m_socket.async_send_to( buffer(m_buffer.get_data_ptr(), m_buffer.get_data_size()),
                            m_endpoint,
                            boost::bind( &net::broadcast_sender::handler_send,
                                         this, _1, _2) );
}

void net::broadcast_sender::handler_send(const boost::system::error_code &error_code, size_t send_bytes)
{
    (void)send_bytes;

    if(!error_code)
    {
        log::message(log::level::Debug, std::string("UDP: Send complete."));
    }
    else
    {
        log::message(log::level::Warning, std::string("UDP: Error send: ") + std::to_string(error_code.value()));
    }

    do_send();
}


    } // namespace net
} // namespace sim
