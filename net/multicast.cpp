#include "multicast.h"

#include <log/log.h>
#include <base/gate.hpp>

namespace sim
{
    namespace net
    {


using namespace sim::base;

net::multicast::multicast(unsigned int port)
    : m_b_valid(true)
    , m_socket( m_io_service, ip::udp::endpoint(ip::udp::v4(), port) )
    , m_endpoint( ip::address_v4::multicast(), port )
    , m_gate_send( new base::gate_stream<base::raw_data>() )
    , m_gate_receive( new base::gate_stream<receive_data_pair>() )
    , m_buffer()
{
    m_socket.set_option( ip::udp::socket::reuse_address(true) );
    m_socket.set_option( socket_base::multicast(true) );
}

net::multicast::~multicast()
{
    m_b_valid = false;
    m_socket.close();

    m_thread->join();
    delete m_thread;

    delete m_gate_receive;
    delete m_gate_send;

    log::message(log::level::Debug, std::string("Broadcast closed."));
}

boost::shared_ptr<multicast> multicast::create(unsigned int port)
{
    auto multicast
        = boost::shared_ptr<net::multicast>(new net::multicast(port));

    multicast->m_thread = new boost::thread(&net::multicast::run, multicast.get());

    return multicast;
}

void multicast::enable_message_mode(unsigned int messages_queue_length)
{
    delete m_gate_receive;
    delete m_gate_send;

    m_gate_send = new base::gate_message<base::raw_data>
            (messages_queue_length);

    m_gate_receive = new base::gate_message<multicast::receive_data_pair>
            (messages_queue_length);
}

bool multicast::get_message(base::raw_data& raw_data, std::string* address_str_ptr/* = nullptr*/, unsigned short* port_ptr/* = nullptr*/)
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

void net::multicast::send_message(const base::raw_data &raw_data)
{
    m_gate_send->push(raw_data);
}

void net::multicast::run()
{
    do_run(false);
    m_io_service.run();
}

void net::multicast::do_run(bool b_send_priority)
{
    while(m_b_valid)
    {
        if(b_send_priority)
        {
            if(!m_gate_send->empty())
            {
                do_send();
                return;
            }
            else
            {
                if(m_socket.available() > 0)
                {
                    do_receive();
                    return;
                }
            }
        }
        else
        {
            if(m_socket.available() > 0)
            {
                do_receive();
                return;
            }
            else
            {
                if(!m_gate_send->empty())
                {
                    do_send();
                    return;
                }
            }
        }
    }
}

void net::multicast::do_receive()
{
    log::message(log::level::Debug, std::string("UDP: Do receive."));

    m_socket.async_receive_from( buffer(m_buffer.first.get_data_ptr(), m_buffer.first.get_data_size()),
                                 m_buffer.second,
                                 boost::bind( &net::multicast::handler_receive,
                                              this, _1, _2) );
}

void net::multicast::handler_receive(const boost::system::error_code &error_code, size_t receive_bytes)
{
    if(!error_code)
    {
        log::message(log::level::Debug, std::string("UDP: Receive complete."));
        m_gate_receive->push(m_buffer);
        do_run(true);
    }
    else
    {
        log::message(log::level::Debug, std::string("UDP: Error receive: ") + error_code.message());
    }
}

void net::multicast::do_send()
{
    log::message(log::level::Debug, std::string("UDP: Do send."));

    if(m_gate_send->pop(m_buffer.first))
    {
        m_socket.async_send_to( buffer(m_buffer.first.get_data_ptr(), m_buffer.first.get_data_size()),
                                m_endpoint,
                                boost::bind( &net::multicast::handler_send,
                                             this, _1, _2) );
    }
    else
    {
        do_run(false);
    }
}

void net::multicast::handler_send(const boost::system::error_code &error_code, size_t send_bytes)
{
    (void)send_bytes;

    if(!error_code)
    {
        log::message(log::level::Debug, std::string("UDP: Send complete."));
        do_run(false);
    }
    else
    {
        log::message(log::level::Debug, std::string("UDP: Error send: ") + error_code.message());
    }
}


    } // namespace net
} // namespace sim
