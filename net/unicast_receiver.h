#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/asio.hpp>

#include <log/log.h>
#include <base/raw_data.h>
#include <base/queue.hpp>


namespace sim
{
    namespace net
    {

using namespace boost::asio;

class SIMLIB_API tcp_receiver : public boost::noncopyable
{
    tcp_receiver(unsigned short port, unsigned int queury_length, size_t data_size);

public:
    ~tcp_receiver();

    static boost::shared_ptr<net::tcp_receiver> create(unsigned short port, unsigned int queury_length = 128, size_t data_size = 1024);

    bool get_message(base::raw_data& raw_data, std::string* address_str_ptr = nullptr, unsigned short* port_ptr = nullptr);

    template <typename T>
    bool get_message(T& message, std::string* address_str_ptr = nullptr, unsigned short* port_ptr = nullptr)
    {
        base::raw_data raw_data;
        if(get_message(raw_data, address_str_ptr, port_ptr))
        {
            if(raw_data.get_data(message))
            {
                return true;
            }
            else
            {
                std::string text
                        = std::string("TCP: can't get message: output type: ")
                        + std::string(typeid(T).name())
                        + std::string(" size: ")
                        + std::to_string(sizeof(T))
                        + std::string("; input message size: ")
                        + std::to_string(raw_data.get_data_size());

                log::message(log::level::Warning, text);
                return false;
            }
        }

        return false;
    }

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

typedef boost::shared_ptr<net::tcp_receiver> tcp_receiver_shptr;


    } // namespace net
} // namespace sim


#endif // TCP_RECEIVER_H