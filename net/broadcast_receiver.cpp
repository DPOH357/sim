#include "broadcast_receiver.h"

#include <log/log.h>
#include <base/gate.hpp>

namespace sim
{
    namespace net
    {


using namespace sim::base;

net::broadcast_receiver::broadcast_receiver(unsigned short port)
    : m_b_valid(true)
    , m_socket( m_io_service, ip::udp::endpoint(ip::udp::v4(), port) )
    , m_endpoint( ip::address_v4::broadcast(), port )
    , m_gate_receive( new base::gate_stream<receive_data_pair>() )
{
    m_socket.set_option( ip::udp::socket::reuse_address(true) );
    m_socket.set_option( socket_base::broadcast(true) );

    m_buffer.first.reserve(1024);
}

net::broadcast_receiver::~broadcast_receiver()
{
    m_b_valid = false;
    m_socket.close();

    m_thread->join();
    delete m_thread;

    delete m_gate_receive;

    log::message(log::level::Debug, std::string("Broadcast receiver closed."));
}

boost::shared_ptr<broadcast_receiver> broadcast_receiver::create(unsigned short port)
{
    auto broadcast_receiver
        = net::broadcast_receiver_ptr(new net::broadcast_receiver(port));

    broadcast_receiver->m_thread = new boost::thread(&net::broadcast_receiver::run, broadcast_receiver.get());

    return broadcast_receiver;
}

void broadcast_receiver::enable_message_mode(unsigned int messages_queue_length)
{
    delete m_gate_receive;

    m_gate_receive = new base::gate_message<broadcast_receiver::receive_data_pair>
            (messages_queue_length);
}

bool broadcast_receiver::get_message(base::raw_data& raw_data, std::string* address_str_ptr/* = nullptr*/, unsigned short* port_ptr/* = nullptr*/)
{
    if(m_gate_receive->pop(m_tmp_receive_data_pair))
    {
        raw_data = m_tmp_receive_data_pair.first;

        if(address_str_ptr)
        {
            *address_str_ptr = m_tmp_receive_data_pair.second.address().to_string();
        }

        if(port_ptr)
        {
            *port_ptr = m_tmp_receive_data_pair.second.port();
        }

        return true;
    }

    return false;
}

void net::broadcast_receiver::run()
{
    do_receive();
    m_io_service.run();
}


void net::broadcast_receiver::do_receive()
{
    log::message(log::level::Debug, std::string("UDP: Do receive."));

    m_socket.async_receive_from( buffer(m_buffer.first.get_data_ptr(), m_buffer.first.get_data_size()),
                                 m_buffer.second,
                                 boost::bind( &net::broadcast_receiver::handler_receive,
                                              this, _1, _2) );
}

void net::broadcast_receiver::handler_receive(const boost::system::error_code &error_code, size_t receive_bytes)
{
    (void)receive_bytes;
    if(!error_code)
    {
        log::message(log::level::Debug, std::string("UDP: Receive complete."));
        m_buffer.first.set_data_size(receive_bytes);
        m_gate_receive->push(m_buffer);
    }
    else
    {
        log::message(log::level::Debug, std::string("UDP: Error receive: ") + std::to_string(error_code.value()));
    }

    do_receive();
}


    } // namespace net
} // namespace sim
