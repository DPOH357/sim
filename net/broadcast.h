#ifndef BROADCAST_H
#define BROADCAST_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/asio.hpp>

#include <base/raw_data_queue.h>

namespace sim
{
    namespace net
    {


using namespace boost::asio;

// as sender
class broadcast : public boost::enable_shared_from_this<broadcast>
                , public boost::noncopyable
{
    broadcast(unsigned int port, size_t data_size_default);

public:
    ~broadcast();

    static boost::shared_ptr<broadcast> create(unsigned int port, size_t data_size_default);

    void send_message(const base::raw_data& raw_data);

private:
    void run();
    void do_send();
    void handler_send( const boost::system::error_code &error_code
                     , std::size_t send_bytes );

private:
    io_service              m_io_service;
    boost::thread*          m_thread;
    ip::udp::socket         m_socket;
    ip::udp::endpoint       m_endpoint;
    base::raw_data_queue    m_queue_send;
    base::raw_data          m_buffer;
};


    } // namespace net
} // namespace sim

#endif // BROADCAST_H
