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
        std::string text("Multicast sender: ");
        text += "generate address from string: """;
        text += multicast_address_str + """ - ";
        text += error_code.message();
        log::message(log::level::Error, text);

        return;
    }

//#ifndef WIN32
    const std::string listen_address_str("0.0.0.0");
    ip::address listen_address = ip::address::from_string(
        listen_address_str, error_code);
    if(error_code)
    {
        std::string text("Multicast client: ");
        text += "Invalid listen address: " + listen_address_str;
        log::message(log::level::Error, text);
        return;
    }
    boost::asio::ip::udp::endpoint listen_endpoint(
        listen_address, multicast_port);
    /*
#else
    boost::asio::ip::udp::endpoint listen_endpoint(
        multicast_address, multicast_port);
#endif
    */

    m_socket.open(
        listen_endpoint.protocol(), error_code);
    if(error_code)
    {
        std::string text("Multicast client: open socket - ");
        text += error_code.message();
        log::message(log::level::Error, text);
        return;
    }

    m_socket.set_option(
        ip::udp::socket::reuse_address(true), error_code);
    if(error_code)
    {
        std::string text("Multicast client: set reuse address socket option - ");
        text += error_code.message();
        log::message(log::level::Error, text);
        return;
    }

    m_socket.bind(
        listen_endpoint);

    m_socket.set_option( ip::multicast::join_group(multicast_address) , error_code);
    if(error_code)
    {
        std::string text("Multicast client: set join group socket option - ");
        text += error_code.message();
        log::message(log::level::Error, text);
        return;
    }

    m_buffer_pair.first.reserve(1024);

    log::message(log::level::Debug, "Multicast client created");
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

bool multicast_client::get_message(
        base::raw_data &raw_data,
        std::string *address_str_ptr/* = nullptr*/,
        unsigned short* port_ptr/* = nullptr*/)
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
    if(m_b_valid)
    {
        do_receive();
    }
}

void net::multicast_client::do_receive()
{
    log::message(
        log::level::Debug,
        std::string("Multicast client: Do receive."));

    m_socket.async_receive_from( buffer(
                                     m_buffer_pair.first.get_data_ptr(),
                                     m_buffer_pair.first.get_reserved_size()),
                                 m_buffer_pair.second,
                                 boost::bind( &multicast_client::handler_receive,
                                              this, _1, _2) );
}

void net::multicast_client::handler_receive(
        const system::error_code &error_code,
        size_t receive_bytes)
{
    if(!error_code)
    {
        m_buffer_pair.first.set_data_size(receive_bytes);
        m_gate_receive->push(m_buffer_pair);
        log::message(log::level::Debug,
                     std::string("Multicast client: Receive complete."));
    }
    else
    {
        log::message(log::level::Debug,
                     std::string("Multicast client: Error receive: ")
                        + error_code.message());
    }

    do_run();
}


    } // namespace net
} // namespace sim
