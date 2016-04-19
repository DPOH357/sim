#include "broadcast.h"

#include <base/tools.h>

sim::net::broadcast::broadcast(unsigned int port, size_t data_size_default)
    : m_socket( m_io_service, ip::udp::endpoint(ip::udp::v4(), 0) )
    , m_endpoint( ip::address_v4::broadcast(), port )
    , m_queue_send(data_size_default)
    , m_buffer(data_size_default)
{
    m_socket.set_option( ip::udp::socket::reuse_address(true) );
    m_socket.set_option( socket_base::broadcast(true) );
}

sim::net::broadcast::~broadcast()
{
    m_socket.close();

    m_thread->join();
    delete m_thread;
}

boost::shared_ptr<sim::net::broadcast> sim::net::broadcast::create(unsigned int port, size_t data_size_default)
{
    auto broadcast
        = boost::shared_ptr<net::broadcast>(new net::broadcast(port, data_size_default));

    broadcast->m_thread = new boost::thread(&net::broadcast::run, broadcast);

    return broadcast;
}

void sim::net::broadcast::send_message(const sim::base::raw_data &raw_data)
{
    m_queue_send.push(raw_data);
}

void sim::net::broadcast::run()
{
    do_send();
    m_io_service.run();
}

void sim::net::broadcast::do_send()
{
    while(!m_queue_send.pop(m_buffer))
    {
        boost::this_thread::sleep_for( boost::chrono::milliseconds(20) );
    }

    m_socket.async_send_to( buffer(m_buffer.get_data_ptr(), m_buffer.get_data_size()),
                            m_endpoint,
                            boost::bind( &net::broadcast::handler_send
                                       , shared_from_this(), _1, _2) );
}

void sim::net::broadcast::handler_send(const boost::system::error_code &error_code, size_t send_bytes)
{
    (void)send_bytes;

    if(error_code)
    {
        base::log::message(error_code.message());
    }

    do_send();
}
