#ifndef BROADCAST_H
#define BROADCAST_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/asio.hpp>

#include <base/raw_data.h>
#include <base/queue.hpp>

namespace sim
{
    namespace net
    {


using namespace boost::asio;

class broadcast : public boost::enable_shared_from_this<broadcast>
                , public boost::noncopyable
{
    typedef std::pair<base::raw_data, ip::udp::endpoint> receive_data_pair;

    broadcast(unsigned int port);

public:
    ~broadcast();

    static boost::shared_ptr<broadcast> create(unsigned int port);

    bool get_message(base::raw_data& raw_data, ip::udp::endpoint* endpoint_sender_ptr = nullptr);

    void send_message(const base::raw_data& raw_data);

private:
    void run();

    void do_run(bool b_send_priority);

    void do_receive();

    void handler_receive( const boost::system::error_code &error_code
                        , std::size_t receive_bytes);

    void do_send();

    void handler_send( const boost::system::error_code &error_code
                     , std::size_t sended_bytes);

private:
    boost::atomic<bool>     m_b_valid;
    io_service              m_io_service;
    boost::thread*          m_thread;
    ip::udp::socket         m_socket;
    ip::udp::endpoint       m_endpoint;
    base::queue<base::raw_data>
                            m_queue_send;
    base::queue<receive_data_pair>
                            m_queue_receive;

    receive_data_pair       m_buffer;
    receive_data_pair       m_tmp_receive_data_pair;
};

typedef boost::shared_ptr<broadcast> broadcast_ptr;


    } // namespace net
} // namespace sim

#endif // BROADCAST_H
