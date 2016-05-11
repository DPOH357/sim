#include "tcp_receiver.h"

#include <base/tools.h>

namespace sim
{
    namespace net
    {

using namespace sim::base;

tcp_receiver::tcp_receiver(unsigned short port, unsigned int queury_length, size_t data_size)
    : m_acceptor(m_io_service, ip::tcp::v4(), port)
    , m_socket(m_io_service)
    , m_queue_receive_data(queury_length)
    , m_buffer(data_size)
    , m_b_valid(true)
{
}

tcp_receiver::~tcp_receiver()
{
    m_b_valid = false;
    m_socket.close();

    m_thread->join();
    delete m_thread;
}

boost::shared_ptr<tcp_receiver> tcp_receiver::create(unsigned short port, unsigned int queury_length/* = 128*/, size_t data_size/* = 1024*/)
{
    auto tcp_receiver
            = boost::shared_ptr<net::tcp_receiver>( new net::tcp_receiver(port, queury_length, data_size) );

    tcp_receiver->m_thread = new boost::thread( boost::bind(&net::tcp_receiver::run, tcp_receiver));

    return tcp_receiver;
}

bool tcp_receiver::get_message(base::raw_data &raw_data, boost::asio::ip::tcp::endpoint *endpoint_ptr)
{
    tcp_receiver::receive_data receive_data;
    bool ret =  m_queue_receive_data.pop(receive_data);
    if(ret)
    {
        raw_data = receive_data.raw_data;
        if(endpoint_ptr)
        {
            *endpoint_ptr = receive_data.endpoint;
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
    m_acceptor.async_accept( m_socket,
                             boost::bind( &net::tcp_receiver::handler_accept, shared_from_this(), _1) );
}

void tcp_receiver::handler_accept(const boost::system::error_code &error_code)
{
    if(!error_code)
    {
        do_receive();
    }
    else
    {
        m_b_valid = false;

        log::message( log::level::Error,
                      std::string("TCP: Error accept (")
                    + std::to_string(error_code.value())
                    + std::string("): ")
                    + error_code.message());
    }
}

void tcp_receiver::do_receive()
{
    m_socket.async_receive( buffer(m_buffer.get_data_ptr(), m_buffer.get_data_size()),
                            boost::bind( &net::tcp_receiver::handler_receive
                                       , shared_from_this(), _1, _2));
}

void tcp_receiver::handler_receive(const boost::system::error_code &error_code, size_t receive_bytes)
{
    if(!error_code)
    {
        log::message(log::level::Debug, std::string("TCP: Receive complete."));

        tcp_receiver::receive_data receive_data;
        receive_data.raw_data.set(m_buffer.get_data_ptr(), receive_bytes);
        receive_data.endpoint = m_socket.local_endpoint();
        m_queue_receive_data.push(receive_data);

        do_accept();
    }
    else
    {
        m_b_valid = false;
        log::message(log::level::Debug, std::string("TCP: Error receive: ") + error_code.message());
    }
}

    } // namespace net
} // namespace sim
