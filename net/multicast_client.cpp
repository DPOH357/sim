#include "multicast_client.h"

#include <log/log.h>
#include <base/gate.hpp>

namespace sim
{
    namespace net
    {


using namespace sim::base;

net::multicast_client::multicast_client(
        const std::string& multicast_address_str,
        unsigned int multicast_port)
    : m_b_valid(true)
    , m_socket(m_io_service)
    , m_gate_receive( new base::gate_stream<multicast_client::receive_data_pair>() )
{
    boost::system::error_code error_code;

    ip::address multicast_address = ip::address::from_string(
        multicast_address_str, error_code);
    if(error_code)
    {
        std::string text("Multicast client: ");
        text += "Invalid multicast address: ";
        text += multicast_address_str;
        log::message(log::level::Error, text);
    }

#if WIN32
    ip::address address = ip::address::from_string(
        "0.0.0.0");
    boost::asio::ip::udp::endpoint listen_endpoint(
        address, multicast_port);
#else
    boost::asio::ip::udp::endpoint listen_endpoint(
        multicast_address, multicast_port);
#endif

    m_socket.open(
        listen_endpoint.protocol());
    m_socket.set_option(
        ip::udp::socket::reuse_address(true));
    m_socket.bind(
        listen_endpoint);
    m_socket.set_option(
        ip::multicast::join_group(multicast_address));
}

net::multicast_client::~multicast_client()
{
    m_b_valid = false;
    m_socket.close();

    m_thread->join();
    delete m_thread;

    delete m_gate_receive;

    log::message(log::level::Debug,
        std::string("Multicast client closed."));
}

boost::shared_ptr<multicast_client> multicast_client::create(
        const std::string& multicast_address_str,
        unsigned int multicast_port)
{
    auto multicast_client
        = boost::shared_ptr<net::multicast_client>(
            new net::multicast_client(
                multicast_address_str,
                multicast_port));

    multicast_client->m_thread = new boost::thread(
        &net::multicast_client::run,
        multicast_client.get());

    return multicast_client;
}

bool multicast_client::get_message(base::raw_data &raw_data, std::string* address_str_ptr/* = nullptr*/, unsigned short* port_ptr/* = nullptr*/)
{
    bool ret =  m_gate_receive->pop(m_tmp_receive_data_pair);
    if(ret)
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
    }

    return ret;
}

void multicast_client::run()
{
    do_run();
    m_io_service.run();
}

void multicast_client::do_run()
{
    while(m_b_valid)
    {
        if(!m_gate_receive->empty())
        {
            do_receive();
        }
    }
}

void net::multicast_client::do_receive()
{
    log::message(
        log::level::Debug,
        std::string("Multicast client: Do receive."));

    m_socket.async_receive_from( buffer(
                                     m_buffer.first.get_data_ptr(),
                                     m_buffer.first.get_data_size()),
                                     m_buffer.second,
                                 boost::bind( &net::multicast_client::handler_receive,
                                              this, _1, _2) );
}

void net::multicast_client::handler_receive(
        const boost::system::error_code &error_code,
        size_t receive_bytes)
{
    (void)receive_bytes;
    if(!error_code)
    {
        log::message(log::level::Debug,
                     std::string("Multicast client: Receive complete."));
        m_gate_receive->push(m_buffer);
        do_run();
    }
    else
    {
        log::message(log::level::Debug,
                     std::string("Multicast client: Error receive: ")
                        + error_code.message());
    }
}


    } // namespace net
} // namespace sim
