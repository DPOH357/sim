#include "multicast_server.h"

#include <log/log.h>
#include <base/gate.hpp>

namespace sim
{
    namespace net
    {


using namespace sim::base;

net::multicast_server::multicast_server(const std::string& multicast_address_str,
        unsigned short port)
    : m_b_valid(true)
    , m_socket(m_io_service)
    , m_gate_send( new base::gate_stream<base::raw_data>() )
{
    boost::system::error_code error_code;

    ip::address address = ip::address::from_string(
                multicast_address_str,
                error_code);
    if(!error_code)
    {
        m_endpoint.address(address);
        m_endpoint.port(port);

        m_socket.open(m_endpoint.protocol());

        m_socket.set_option( ip::udp::socket::reuse_address(true) );
    }
    else
    {
        std::string text("Multicast sender: ");
        text += "generate address from string """;
        text += multicast_address_str + """ - ";
        text += error_code.message();
        log::message(log::level::Error, text);
    }

    log::message(log::level::Debug, "Multicast server created");

}

net::multicast_server::~multicast_server()
{
    m_b_valid = false;
    m_socket.close();

    m_thread->join();
    delete m_thread;

    delete m_gate_send;

    log::message(log::level::Debug,
                 std::string("Multicast server closed."));
}

boost::shared_ptr<multicast_server> multicast_server::create(const std::string& multicast_address_str,
        unsigned short port)
{
    auto multicast_server
        = boost::shared_ptr<net::multicast_server>(
                new net::multicast_server(multicast_address_str, port));

    multicast_server->m_thread = new boost::thread(
                &net::multicast_server::run,
                multicast_server.get());

    return multicast_server;
}

void net::multicast_server::send_message(
        const base::raw_data &raw_data)
{
    m_gate_send->push(raw_data);
}

void multicast_server::run()
{
    do_run();
    m_io_service.run();
}

void multicast_server::do_run()
{
    while(m_b_valid)
    {
        if(!m_gate_send->empty())
        {
            do_send();
        }
    }
}

void net::multicast_server::do_send()
{
    log::message(log::level::Debug,
                 std::string("Multicast server: Do send."));

    if(m_gate_send->pop(m_buffer))
    {
        m_socket.async_send_to( buffer(m_buffer.get_data_ptr(),
                                       m_buffer.get_data_size()),
                                m_endpoint,
                                boost::bind( &net::multicast_server::handler_send,
                                             this, _1, _2) );
    }
    else
    {
        do_run();
    }
}

void net::multicast_server::handler_send(
        const boost::system::error_code &error_code,
        size_t send_bytes)
{
    (void)send_bytes;

    if(!error_code)
    {
        log::message(log::level::Debug,
                     std::string("Multicast server: Send complete."));
    }
    else
    {
        log::message(log::level::Debug,
                     std::string("Multicast server: Error send: ")
                        + error_code.message());
    }

    do_run();
}


    } // namespace net
} // namespace sim
