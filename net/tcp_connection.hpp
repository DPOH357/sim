#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/asio.hpp>

#include "gate.hpp"


namespace sim
{
    namespace net
    {

template <typename T>
class tcp_connection : public boost::enable_shared_from_this<net::tcp_connection<T> >
                     , public boost::noncopyable
{
    tcp_connection(unsigned int port, net::gate_interface<T>* gate_send, net::gate_interface<T>* gate_receive)
        : m_socket(m_io_service)
        , m_gate_receive(gate_receive)
        , m_gate_send(gate_send)
        , m_b_valid(true)
    {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        boost::asio::ip::tcp::acceptor acceptor(m_io_service, endpoint);

        acceptor.async_accept( m_socket, boost::bind( &net::tcp_connection<T>::handler_accept, shared_from_this) );
    }

    tcp_connection(boost::asio::ip::address address, unsigned int port, net::gate_interface<T>* gate_send, net::gate_interface<T>* gate_receive)
        : m_socket(m_io_service)
        , m_gate_receive(gate_receive)
        , m_gate_send(gate_send)
        , m_b_valid(true)
    {
        boost::asio::ip::tcp::endpoint endpoint(address, port);
        m_socket.async_connect(endpoint, boost::bind( &net::tcp_connection<T>::handler_connect, shared_from_this) );
    }

public:
    ~tcp_connection()
    {
        m_socket.close();
        m_b_valid = false;

        m_thread->join();
        delete m_thread;

        delete m_gate;
    }

    static boost::shared_ptr<net::tcp_connection<T>> create(unsigned int port, net::gate_interface<T>* gate_output, net::gate_interface<T>* gate_input)
    {
        auto tcp_connection
            = boost::shared_ptr<net::tcp_connection<T>>(new net::tcp_connection<T>(port, gate_output, gate_input));

        tcp_connection->m_thread = new boost::thread(&net::tcp_connection<T>::run, tcp_connection);

        return tcp_connection;
    }

    static boost::shared_ptr<net::tcp_connection<T>> create(boost::asio::ip::address address, unsigned int port, net::gate_interface<T>* gate_output, net::gate_interface<T>* gate_input)
    {
        auto tcp_connection
            = boost::shared_ptr<net::tcp_connection<T>>(new net::tcp_connection<T>(address, port, gate_output, gate_input));

        tcp_connection->m_thread = new boost::thread(&net::tcp_connection<T>::run, tcp_connection);

        return tcp_connection;
    }

    bool get_message(T& out_message)
    {
        return m_gate_receive->pop(out_message);
    }

    void send_message(const T& message)
    {
        m_gate_send->push(message);
    }

    bool valid() const
    {
        return m_b_valid;
    }

private:
    void run()
    {
        m_io_service.run();
    }

    void do_run(bool b_send_priority)
    {
        while(m_b_valid)
        {
            if(b_send_priority)
            {
                if(!m_gate_output->empty())
                {
                    do_send();
                }
                else
                {
                    if(m_socket.available() > 0)
                    {
                        do_receive();
                    }
                }
            }
            else
            {
                if(m_socket.available() > 0)
                {
                    do_receive();
                }
                else
                {
                    if(!m_gate_output->empty())
                    {
                        do_send();
                    }
                }
            }
        }
    }

    void handler_accept(const boost::system::error_code& error_code)
    {
        if(error_code)
        {
            m_b_valid = false;
        }
        else
        {
            do_run(false);
        }
    }

    void handler_connect(const boost::system::error_code& error_code)
    {
        if(error_code)
        {
            m_b_valid = false;
        }
        else
        {
            do_run(false);
        }
    }

    void do_receive()
    {
        m_socket.async_receive(boost::asio::buffer(&m_buffer, sizeof(m_buffer)),
                               boost::bind(&net::tcp_connection<T>::handler_receive)
                                          , shared_from_this(), _1, _2);
    }

    void handler_receive( const boost::system::error_code &error_code
                        , std::size_t receive_bytes)
    {
        if(!error_code)
        {
            m_gate_input->push(m_buffer);
            do_run(true);
        }
        else
        {
            m_b_valid = false;
            LOG_MESSAGE(std::string("TCP: Error receive: ") + error_code.message());
        }
    }

    void do_send()
    {
        m_gate_output->pop(m_buffer);
        m_socket.async_write_some( boost::asio::buffer(&m_buffer, sizeof(m_buffer)),
                                   boost::bind(&net::tcp_connection<T>::handler_send)
                                              , shared_from_this(), _1, _2);
    }

    void handler_send( const boost::system::error_code &error_code
                     , std::size_t sended_bytes)
    {
        if(!error_code)
        {
            do_run(false);
        }
        else
        {
            m_b_valid = false;
            LOG_MESSAGE(std::string("TCP: Error send: ") + error_code.message());
        }
    }

private:
    boost::atomic<bool>     m_b_valid;
    boost::asio::io_service m_io_service;
    boost::thread*          m_thread;
    boost::asio::ip::udp::socket    m_socket;
    net::gate_interface<T>* m_gate_receive;
    net::gate_interface<T>* m_gate_send;
    T                       m_buffer;
};



    } // namespace net
} // namespace sim


#endif // TCP_CONNECTION_H
