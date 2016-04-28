#include "broadcast.h"

#include <base/tools.h>

namespace sim
{
    namespace net
    {


using namespace sim::base;

net::broadcast::broadcast(unsigned int port)
    : m_b_valid(true)
    , m_socket( m_io_service, ip::udp::endpoint(ip::udp::v4(), port) )
    , m_endpoint( ip::address_v4::broadcast(), port )
    , m_queue_send(100)
    , m_queue_receive(100)
    , m_buffer()
{
    m_socket.set_option( ip::udp::socket::reuse_address(true) );
    m_socket.set_option( socket_base::broadcast(true) );
}

net::broadcast::~broadcast()
{
    m_socket.close();

    m_thread->join();
    delete m_thread;
}

boost::shared_ptr<broadcast> broadcast::create(unsigned int port)
{
    auto broadcast
        = boost::shared_ptr<net::broadcast>(new net::broadcast(port));

    broadcast->m_thread = new boost::thread(&net::broadcast::run, broadcast);

    return broadcast;
}

bool broadcast::get_message(base::raw_data& raw_data, ip::udp::endpoint* endpoint_sender_ptr/* = nullptr*/)
{
    if(m_queue_receive.pop(m_tmp_receive_data_pair))
    {
        raw_data = m_tmp_receive_data_pair.first;

        if(endpoint_sender_ptr)
        {
            *endpoint_sender_ptr = m_tmp_receive_data_pair.second;
        }
    }

    return false;
}

void net::broadcast::send_message(const base::raw_data &raw_data)
{
    m_queue_send.push(raw_data);
}

void net::broadcast::run()
{
    do_run(false);
    m_io_service.run();
}

void net::broadcast::do_run(bool b_send_priority)
{
    while(m_b_valid)
    {
        if(b_send_priority)
        {
            if(!m_queue_send.is_empty())
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
                if(!m_queue_send.is_empty())
                {
                    do_send();
                    return;
                }
            }
        }
    }
}

void net::broadcast::do_receive()
{
    log::message(log::level::Info, std::string("UDP: Do receive."));

    m_socket.async_receive_from( buffer(m_buffer.first.get_data_ptr(), m_buffer.first.get_data_size()),
                                 m_buffer.second,
                                 boost::bind( &net::broadcast::handler_receive,
                                              shared_from_this(), _1, _2) );
}

void net::broadcast::handler_receive(const boost::system::error_code &error_code, size_t receive_bytes)
{
    if(!error_code)
    {
        log::message(log::level::Info, std::string("UDP: Receive complete."));
        m_queue_receive.push(m_buffer);
        do_run(true);
    }
    else
    {
        m_b_valid = false;
        log::message(log::level::Info, std::string("UDP: Error receive: ") + error_code.message());
    }
}

void net::broadcast::do_send()
{
    log::message(log::level::Info, std::string("UDP: Do send."));

    if(m_queue_send.pop(m_buffer.first))
    {
        m_socket.async_send_to( buffer(m_buffer.first.get_data_ptr(), m_buffer.first.get_data_size()),
                                m_endpoint,
                                boost::bind( &net::broadcast::handler_send,
                                             shared_from_this(), _1, _2) );
    }
    else
    {
        do_run(false);
    }
}

void net::broadcast::handler_send(const boost::system::error_code &error_code, size_t send_bytes)
{
    (void)send_bytes;

    if(!error_code)
    {
        log::message(log::level::Info, std::string("UDP: Send complete."));
        m_queue_send.pop();
        do_send();
    }
    else
    {
        log::message(log::level::Info, std::string("UDP: Error send: ") + error_code.message());
    }
}


    } // namespace net
} // namespace sim
