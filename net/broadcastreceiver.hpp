#ifndef BEACON_H
#define BEACON_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/asio.hpp>
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
#include <iostream>


namespace net
{


#define LOG_MESSAGE(x) std::cout << x << std::endl;


template <typename T>
class broadcast_receiver : public boost::enable_shared_from_this<net::broadcast_receiver<T> >
                         , public boost::noncopyable
{
    enum { queue_max_size = 256 };

    broadcast_receiver(unsigned int port)
        : m_socket( m_io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port) )
        , m_endpoint()
    {
        m_socket.set_option( boost::asio::socket_base::broadcast(true) );
    }

public:
    ~broadcast_receiver()
    {
        m_socket.close();

        m_thread->join();
        delete m_thread;
    }

    static boost::shared_ptr<net::broadcast_receiver<T>> create(unsigned int port)
    {
        auto broadcast_receiver
            = boost::shared_ptr<net::broadcast_receiver<T>>(new net::broadcast_receiver<T>(port));

        broadcast_receiver->m_thread = new boost::thread(&net::broadcast_receiver<T>::run, broadcast_receiver);

        return broadcast_receiver;
    }

    bool get_message(T& out_message)
    {
        boost::lock_guard<boost::mutex> locker(m_mutex);
        if(!m_queue.empty())
        {
            out_message = m_queue.front();
            m_queue.pop();
            return true;
        }

        return false;
    }

private:
    void run()
    {
        do_receive();
        m_io_service.run();
    }

    void do_receive()
    {
        m_socket.async_receive_from( boost::asio::buffer( &m_buffer
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
            boost::lock_guard<boost::mutex> locker(m_mutex);
            if(m_queue.size() >= queue_max_size)
            {
                m_queue.pop();
                LOG_MESSAGE(std::string("Queue inputs messages is full."));
            }

            m_queue.push(m_buffer);
        }
        else
        {
            LOG_MESSAGE(std::string("Error receive - ") + error_code.message());
        }

        do_receive();
    }

private:
    boost::mutex            m_mutex;
    boost::asio::io_service m_io_service;
    boost::thread*          m_thread;
    boost::asio::ip::udp::socket    m_socket;
    boost::asio::ip::udp::endpoint  m_endpoint;
    std::queue<T>           m_queue;
    T                       m_buffer;
};


} // namespace net


#endif // BEACON_H
