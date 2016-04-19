#ifndef BROADCAST_SENDER_H
#define BROADCAST_SENDER_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "gate.hpp"


namespace sim
{
    namespace net
    {

using namespace boost::asio;


template < class T >
class broadcast_sender : public boost::enable_shared_from_this<net::broadcast_sender<T> >
                       , public boost::noncopyable
{
    broadcast_sender(unsigned int port, net::gate_interface<T>* gate)
        : m_socket( m_io_service, ip::udp::endpoint(ip::udp::v4(), 0) )
        , m_endpoint( ip::address_v4::broadcast(), port )
        , m_gate(gate)
    {
        m_socket.set_option( ip::udp::socket::reuse_address(true) );
        m_socket.set_option( socket_base::broadcast(true) );
    }

public:
    ~broadcast_sender()
    {
        m_socket.close();

        m_thread->join();
        delete m_thread;

        delete m_gate;
    }

    static boost::shared_ptr<net::broadcast_sender<T>> create(unsigned int port, net::gate_interface<T>* gate)
    {
        auto broadcast_sender
            = boost::shared_ptr<net::broadcast_sender<T>>(new net::broadcast_sender<T>(port, gate));

        broadcast_sender->m_thread = new boost::thread(&net::broadcast_sender<T>::run, broadcast_sender);

        return broadcast_sender;
    }

    void send(const T& message)
    {
        m_gate->push(message);
    }

private:
    void run()
    {
        do_send();
        m_io_service.run();
    }

    void do_send()
    {
        while(!m_gate->pop(m_buffer))
        {
            boost::this_thread::sleep_for( boost::chrono::milliseconds(20) );
        }

        m_socket.async_send_to( buffer( &m_buffer,
                                                     sizeof(T) ),
                                m_endpoint,
                                boost::bind( &net::broadcast_sender<T>::handler_send
                                           , shared_from_this(), _1, _2) );
    }

    void handler_send( const boost::system::error_code &error_code
                     , std::size_t send_bytes )
    {
        (void)send_bytes;

        if(error_code)
        {
            base::log::message(std::string("UDP: Error sending: ") + error_code.message());
        }

        do_send();
    }

private:
    net::gate_interface<T>*         m_gate;
    T                               m_buffer;
    io_service                      m_io_service;
    boost::thread*                  m_thread;
    ip::udp::socket    m_socket;
    ip::udp::endpoint  m_endpoint;
};


    } // namespace net
} // namespace sim


#endif // BEACON_H
