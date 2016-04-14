#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/asio.hpp>

#include "gate.hpp"

#include "tools/tools.h"


namespace sim
{
    namespace net
    {

using namespace boost::asio;

class tcp_connection : public boost::enable_shared_from_this<net::tcp_connection>
                     , public boost::noncopyable
{
    tcp_connection(size_t data_size);

public:
    ~tcp_connection();

    static boost::shared_ptr<net::tcp_connection> create_wait_connection(unsigned int port, size_t data_size);

    static boost::shared_ptr<net::tcp_connection> create_connect(ip::address address, unsigned int port, size_t data_size);

    bool get_message(tool::raw_data& raw_data);

    void send_message(const tool::raw_data& raw_data);

    bool valid() const;

private:
    void run(ip::address address, unsigned int port);

    void run(unsigned int port);

    void do_run(bool b_send_priority);

    void handler_accept(const boost::system::error_code& error_code);

    void handler_connect(const boost::system::error_code& error_code);

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
    ip::tcp::socket         m_socket;
    tool::raw_data_queue    m_queue_receive;
    tool::raw_data_queue    m_queue_send;
    tool::raw_data          m_buffer;
};


    } // namespace net
} // namespace sim


#endif // TCP_CONNECTION_H
