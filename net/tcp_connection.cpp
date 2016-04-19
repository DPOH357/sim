#include "tcp_connection.h"

namespace sim
{
    namespace net
    {

tcp_connection::tcp_connection(size_t data_size)
    : m_socket(m_io_service)
    , m_queue_receive(data_size)
    , m_queue_send(data_size)
    , m_buffer(data_size)
    , m_b_valid(true)
{
}

tcp_connection::~tcp_connection()
{
    m_b_valid = false;
    m_socket.close();

    m_thread->join();
    delete m_thread;
}

boost::shared_ptr<tcp_connection> tcp_connection::create_wait_connection(unsigned int port, size_t data_size)
{
    auto tcp_connection
            = boost::shared_ptr<net::tcp_connection>( new net::tcp_connection(data_size) );

    tcp_connection->m_thread = new boost::thread( boost::bind(&net::tcp_connection::run, tcp_connection, _1), port );

    return tcp_connection;
}

boost::shared_ptr<tcp_connection> tcp_connection::create_connect(ip::address address, unsigned int port, size_t data_size)
{
    auto tcp_connection
            = boost::shared_ptr<net::tcp_connection>(new net::tcp_connection(data_size));

    tcp_connection->m_thread = new boost::thread( boost::bind(&net::tcp_connection::run, tcp_connection, _1, _2), address, port);

    return tcp_connection;
}

bool tcp_connection::get_message(base::raw_data &raw_data)
{
    return m_queue_receive.pop(raw_data);
}

void tcp_connection::send_message(const base::raw_data &raw_data)
{
    m_queue_send.push(raw_data);
}

bool tcp_connection::valid() const
{
    return m_b_valid;
}

void tcp_connection::run(ip::address address, unsigned int port)
{
    ip::tcp::endpoint endpoint(address, port);
    m_socket.async_connect(endpoint,
                           boost::bind( &net::tcp_connection::handler_connect, shared_from_this(), _1) );

    m_io_service.run();
}

void tcp_connection::run(unsigned int port)
{
    ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
    ip::tcp::acceptor acceptor(m_io_service, endpoint);

    acceptor.async_accept( m_socket,
                           boost::bind( &net::tcp_connection::handler_accept, shared_from_this(), _1) );

    m_io_service.run();
}

void tcp_connection::do_run(bool b_send_priority)
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
                if(m_socket.available() >= m_buffer.get_data_size())
                {
                    do_receive();
                    return;
                }
            }
        }
        else
        {
            if(m_socket.available() >= m_buffer.get_data_size())
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

void tcp_connection::handler_accept(const boost::system::error_code &error_code)
{
    if(error_code)
    {
        m_b_valid = false;

        LOG_MESSAGE( std::string("TCP: Error accept (")
                   + std::to_string(error_code.value())
                   + std::string("): ")
                   + error_code.message());
    }
    else
    {
        LOG_MESSAGE(std::string("TCP: Accept valid."));
        do_run(false);
    }
}

void tcp_connection::handler_connect(const boost::system::error_code &error_code)
{
    if(error_code)
    {
        m_b_valid = false;
        LOG_MESSAGE(std::string("TCP: Error connect: ") + error_code.message());
    }
    else
    {
        LOG_MESSAGE(std::string("TCP: Connection valid."));
        do_run(false);
    }
}

void tcp_connection::do_receive()
{
    LOG_MESSAGE(std::string("TCP: Do receive."));
    m_socket.async_receive(buffer(m_buffer.get_data_ptr(), m_buffer.get_data_size()),
                           boost::bind(&net::tcp_connection::handler_receive
                                       , shared_from_this(), _1, _2));
}

void tcp_connection::handler_receive(const boost::system::error_code &error_code, size_t receive_bytes)
{
    if(!error_code)
    {
        LOG_MESSAGE(std::string("TCP: Receive complete."));
        m_queue_receive.push(m_buffer);
        do_run(true);
    }
    else
    {
        m_b_valid = false;
        LOG_MESSAGE(std::string("TCP: Error receive: ") + error_code.message());
    }
}

void tcp_connection::do_send()
{
    LOG_MESSAGE(std::string("TCP: Do send."));
    m_queue_send.front(m_buffer);
    m_socket.async_send( buffer(m_buffer.get_data_ptr(), m_buffer.get_data_size()),
                                boost::bind(&net::tcp_connection::handler_send
                                           , shared_from_this(), _1, _2));
}

void tcp_connection::handler_send(const boost::system::error_code &error_code, size_t sended_bytes)
{
    if(!error_code)
    {
        LOG_MESSAGE(std::string("TCP: Send complete."));
        m_queue_send.pop();
        do_run(false);
    }
    else
    {
        m_b_valid = false;
        LOG_MESSAGE(std::string("TCP: Error send: ") + error_code.message());
    }
}




    } // namespace net
} // namespace sim
