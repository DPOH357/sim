#ifndef BROADCAST_RECEIVER_H
#define BROADCAST_RECEIVER_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/asio.hpp>
#include <functional>

#include "gate.hpp"


namespace sim
{
    namespace net
    {

using namespace boost::asio;

template <typename T>
class broadcast_receiver : public boost::enable_shared_from_this<net::broadcast_receiver<T> >
                         , public boost::noncopyable
{
    broadcast_receiver(unsigned int port, net::gate_interface<T>* gate)
        : m_socket( m_io_service, ip::udp::endpoint(ip::udp::v4(), port) )
        , m_endpoint()
        , m_gate(gate)
    {
        m_socket.set_option( socket_base::broadcast(true) );
    }

public:
    ~broadcast_receiver()
    {
        m_socket.close();

        m_thread->join();
        delete m_thread;

        delete m_gate;
    }

    static boost::shared_ptr<net::broadcast_receiver<T>> create(unsigned int port, net::gate_interface<T>* gate)
    {
        auto broadcast_receiver
            = boost::shared_ptr<net::broadcast_receiver<T>>(new net::broadcast_receiver<T>(port, gate));

        broadcast_receiver->m_thread = new boost::thread(&net::broadcast_receiver<T>::run, broadcast_receiver);

        return broadcast_receiver;
    }

    bool get_message(T& out_message)
    {
        return m_gate->pop(out_message);
    }

private:
    void run()
    {
        do_receive();
        m_io_service.run();
    }

    void do_receive()
    {
        m_socket.async_receive_from( buffer( &m_buffer
                                                        , sizeof(T) ),
                                     m_endpoint,
                                     boost::bind( &net::broadcast_receiver<T>::handler_receive
                                                , shared_from_this(), _1, _2) );
    }

    void handler_receive( const boost::system::error_code &error_code
                        , std::size_t receive_bytes)
    {
        (void)receive_bytes;
        if(!error_code)
        {
            m_gate->push(m_buffer);
        }
        else
        {
            LOG_MESSAGE(std::string("UDP: Error receive: ") + error_code.message());
        }

        do_receive();
    }

private:
    io_service m_io_service;
    boost::thread*          m_thread;
    ip::udp::socket    m_socket;
    ip::udp::endpoint  m_endpoint;
    net::gate_interface<T>* m_gate;
    T                       m_buffer;
};



    } // namespace net
} // namespace sim


#endif // BROADCAST_RECEIVER_H
