#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H

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

class SIMLIB_API tcp_receiver : public boost::enable_shared_from_this<net::tcp_receiver>
                              , public boost::noncopyable
{
    tcp_receiver(unsigned short port, unsigned int queury_length, size_t data_size);

public:
    ~tcp_receiver();

    static boost::shared_ptr<net::tcp_receiver> create(unsigned short port, unsigned int queury_length = 128, size_t data_size = 1024);

    bool get_message(base::raw_data& raw_data, ip::tcp::endpoint* endpoint_ptr = nullptr);

    bool valid() const;

private:
    void run();

    void do_accept();

    void handler_accept(const boost::system::error_code& error_code);

    void do_receive();

    void handler_receive( const boost::system::error_code &error_code
                        , std::size_t receive_bytes);

private:
    struct receive_data
    {
        ip::tcp::endpoint endpoint;
        base::raw_data raw_data;
    };

    boost::atomic<bool>     m_b_valid;
    io_service              m_io_service;
    boost::thread*          m_thread;

    ip::tcp::acceptor       m_acceptor;
    ip::tcp::socket         m_socket;
    base::queue<tcp_receiver::receive_data>    m_queue_receive_data;
    base::raw_data          m_buffer;
};


    } // namespace net
} // namespace sim


#endif // TCP_RECEIVER_H
