#ifndef TCP_SENDER_H
#define TCP_SENDER_H

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

class SIMLIB_API tcp_sender : public boost::enable_shared_from_this<net::tcp_sender>
                            , public boost::noncopyable
{
    tcp_sender(unsigned int queury_length, size_t data_size);

public:
    ~tcp_sender();

    static boost::shared_ptr<net::tcp_sender> create(unsigned int queue_length = 128, size_t data_size = 1024);

    void send_message(const base::raw_data& raw_data, std::string address_str, unsigned short port);

    bool valid() const;

private:
    void run(ip::tcp::endpoint endpoint);

    void run();

    void do_run();

    void do_connect();

    void handler_connect(const boost::system::error_code& error_code);

    void do_send();

    void handler_send( const boost::system::error_code &error_code
                     , std::size_t sended_bytes);

private:
    struct send_data
    {
        ip::tcp::endpoint endpoint;
        base::raw_data raw_data;

        send_data()
        {

        }

        send_data(size_t data_size)
            : raw_data(data_size)
        {

        }
    };

    boost::atomic<bool>     m_b_valid;
    io_service              m_io_service;
    boost::thread*          m_thread;
    ip::tcp::socket         m_socket;
    ip::tcp::endpoint       m_endpoint;
    base::queue<send_data>  m_queue_send;
    send_data               m_buffer;
};

typedef boost::shared_ptr<net::tcp_sender> tcp_sender_shptr;


    } // namespace net
} // namespace sim


#endif // TCP_SENDER_H
