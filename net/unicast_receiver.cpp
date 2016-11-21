#include "unicast_receiver.h"

#include <log/log.h>

namespace sim
{
    namespace net
    {

using namespace sim::base;

tcp_receiver::tcp_receiver(unsigned short port, unsigned int queury_length, size_t data_size)
    : m_acceptor(m_io_service, ip::tcp::endpoint(ip::tcp::v4(), port))
    , m_socket(m_io_service)
    , m_queue_receive_data(queury_length)
    , m_buffer(data_size)
    , m_b_valid(true)
{
}

tcp_receiver::~tcp_receiver()
{
    m_b_valid = false;
    m_acceptor.close();
    m_socket.close();

    m_thread->join();
    delete m_thread;

    log::message(log::level::Debug, std::string("TCP receiver closed."));
}

boost::shared_ptr<net::tcp_receiver> tcp_receiver::create(unsigned short port, unsigned int queury_length/* = 128*/, size_t data_size/* = 1024*/)
{
    auto tcp_receiver = boost::shared_ptr<net::tcp_receiver>(new net::tcp_receiver(port, queury_length, data_size));

    tcp_receiver->m_thread = new boost::thread( boost::bind(&net::tcp_receiver::run, tcp_receiver.get()));

    return tcp_receiver;
}

bool tcp_receiver::get_message(base::raw_data &raw_data, std::string *address_str_ptr, unsigned short *port_ptr)
{
    tcp_receiver::receive_data receive_data;
    bool ret =  m_queue_receive_data.pop(receive_data);
    if(ret)
    {
        raw_data = receive_data.raw_data;

        if(address_str_ptr)
        {
            *address_str_ptr = receive_data.endpoint.address().to_string();
        }

        if(port_ptr)
        {
            *port_ptr = receive_data.endpoint.port();
        }
    }

    return ret;
}

bool tcp_receiver::valid() const
{
    return m_b_valid;
}

void tcp_receiver::run()
{
    do_accept();

    m_io_service.run();
}

void tcp_receiver::do_accept()
{
    log::message(log::level::Debug, std::string("TCP receiver: Accept..."));
    m_acceptor.async_accept( m_socket,
                             boost::bind( &net::tcp_receiver::handler_accept, this, _1) );
}

void tcp_receiver::handler_accept(const boost::system::error_code &error_code)
{
    if(!error_code)
    {
        log::message(log::level::Debug, std::string("TCP receiver: Accept complete."));
        do_receive();
    }
    else
    {
        m_b_valid = false;

        log::message( log::level::Error,
                      std::string("TCP receiver: Error accept (")
                    + std::to_string(error_code.value())
                    + std::string("): ")
                    + error_code.message());
    }
}

void tcp_receiver::do_receive()
{
    log::message(log::level::Debug, std::string("TCP receiver: Receive..."));
    m_socket.async_receive( buffer(m_buffer.get_data_ptr(), m_buffer.get_reserved_size()),
                            boost::bind( &net::tcp_receiver::handler_receive
                                       , this, _1, _2));
}

void tcp_receiver::handler_receive(const boost::system::error_code &error_code, size_t receive_bytes)
{
    if(!error_code)
    {
        log::message(log::level::Debug, std::string("TCP receiver: Receive complete."));

        tcp_receiver::receive_data receive_data;
        receive_data.raw_data.set_data(m_buffer.get_data_ptr(), receive_bytes);
        receive_data.endpoint = m_socket.remote_endpoint();
        m_queue_receive_data.push(receive_data);

        m_socket.close();

        do_accept();
    }
    else
    {
        m_b_valid = false;
        log::message(log::level::Warning, std::string("TCP receiver: Error receive: ") + error_code.message());
    }
}

    } // namespace net
} // namespace sim
