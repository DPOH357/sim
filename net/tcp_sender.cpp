#include "tcp_sender.h"

#include <base/tools.h>

namespace sim
{
    namespace net
    {

using namespace sim::base;

tcp_sender::tcp_sender(unsigned int queury_length, size_t data_size)
    : m_socket(m_io_service)
    , m_queue_send(queury_length)
    , m_buffer(data_size)
    , m_b_valid(true)
{
}

tcp_sender::~tcp_sender()
{
    m_b_valid = false;
    m_socket.close();

    m_thread->join();
    delete m_thread;

    log::message(log::level::Debug, std::string("TCP sender closed."));
}

boost::shared_ptr<tcp_sender> tcp_sender::create(unsigned int queue_length/* = 128*/, size_t data_size/* = 1024*/)
{
    auto tcp_sender
            = boost::shared_ptr<net::tcp_sender>(new net::tcp_sender(queue_length, data_size));

    tcp_sender->m_thread = new boost::thread( boost::bind(&net::tcp_sender::run, tcp_sender));

    return tcp_sender;
}

void tcp_sender::send_message(const base::raw_data& raw_data, std::string address_str, unsigned short port)
{
    boost::system::error_code error_code;
    ip::address address = ip::address::from_string(address_str, error_code);

    if(!error_code)
    {
        tcp_sender::send_data data;
        data.raw_data = raw_data;
        data.endpoint = ip::tcp::endpoint(address, port);

        m_queue_send.push(data);
    }
    else
    {
        log::message(log::level::Warning, std::string("TCP sender: Invalid address: ") + address_str);
    }
}

bool tcp_sender::valid() const
{
    return m_b_valid;
}

void tcp_sender::run()
{
    do_run();

    m_io_service.run();
}

void tcp_sender::do_run()
{
    while(!m_queue_send.pop(m_buffer))
    {
        boost::this_thread::sleep_for( boost::chrono::milliseconds(10) );
    }

    do_connect();
}

void tcp_sender::do_connect()
{
    m_socket.async_connect( m_buffer.endpoint
                          , boost::bind( &net::tcp_sender::handler_connect, shared_from_this(), _1) );
}

void tcp_sender::handler_connect(const boost::system::error_code &error_code)
{
    if(!error_code)
    {
        log::message(log::level::Debug, std::string("TCP sender: Connection valid."));
        do_send();
    }
    else
    {
        m_b_valid = false;
        log::message(log::level::Warning, std::string("TCP sender: Error connect: ") + error_code.message());
        do_run();
    }
}

void tcp_sender::do_send()
{
    log::message(log::level::Debug, std::string("TCP sender: Do send."));
    m_socket.async_send( buffer(m_buffer.raw_data.get_data_ptr(), m_buffer.raw_data.get_data_size()),
                         boost::bind( &net::tcp_sender::handler_send
                                    , shared_from_this(), _1, _2));
}

void tcp_sender::handler_send(const boost::system::error_code &error_code, size_t sended_bytes)
{
    (void)sended_bytes;

    if(!error_code)
    {
        log::message(log::level::Debug, std::string("TCP sender: Send complete."));

        do_run();
    }
    else
    {
        m_b_valid = false;
        log::message(log::level::Warning, std::string("TCP sender: Error send: ") + error_code.message());
    }
}


    } // namespace net
} // namespace sim
