#ifndef BROADCAST_SENDER_H
#define BROADCAST_SENDER_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/asio.hpp>
#include <queue>
#include <atomic>
#include <iostream>


namespace net
{


#define LOG_MESSAGE(x) std::cout << x << std::endl;


template < class T >
class broadcast_sender : public boost::enable_shared_from_this<net::broadcast_sender<T> >
                       , public boost::noncopyable
{
    enum { queue_max_size = 256 };

    broadcast_sender(unsigned int port)
        : m_socket( m_io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0) )
        , m_endpoint( boost::asio::ip::address_v4::broadcast(), port )
    {
        m_socket.set_option( boost::asio::ip::udp::socket::reuse_address(true) );
        m_socket.set_option( boost::asio::socket_base::broadcast(true) );
    }

public:
    ~broadcast_sender()
    {
        m_socket.close();

        m_thread->join();
        delete m_thread;
    }

    static boost::shared_ptr<net::broadcast_sender<T>> create(unsigned int port)
    {
        auto broadcast_sender
            = boost::shared_ptr<net::broadcast_sender<T>>(new net::broadcast_sender<T>(port));

        broadcast_sender->m_thread = new boost::thread(&net::broadcast_sender<T>::run, broadcast_sender);

        return broadcast_sender;
    }

    void send(const T& message)
    {
        boost::unique_lock<boost::mutex> locker(m_mutex);
        m_queue.push(message);
        locker.unlock();
    }

private:
    void run()
    {
        do_send();
        m_io_service.run();
    }

    void do_send()
    {
        boost::unique_lock<boost::mutex> locker(m_mutex);

        while(m_queue.empty())
        {
            locker.unlock();
            boost::this_thread::sleep_for( boost::chrono::milliseconds(20) );
            locker.lock();
        }

        m_socket.async_send_to( boost::asio::buffer( &m_queue.front()
                                                         , sizeof(T) ),
                                m_endpoint,
                                boost::bind( &net::broadcast_sender<T>::handler_send
                                           , shared_from_this(), _1, _2) );
    }

    void handler_send( const boost::system::error_code &error_code
                     , std::size_t send_bytes )
    {
        (void)send_bytes;

        if(!error_code)
        {
            m_queue.pop();
        }
        else
        {
            LOG_MESSAGE(std::string("Error sending - ") + error_code.message());
        }

        do_send();
    }

private:
    boost::mutex        m_mutex;
    std::queue<T>       m_queue;
    T                   m_buffer;
    boost::asio::io_service m_io_service;
    boost::thread*      m_thread;
    boost::asio::ip::udp::socket     m_socket;
    boost::asio::ip::udp::endpoint   m_endpoint;
};


} // namespace net


#endif // BEACON_H
