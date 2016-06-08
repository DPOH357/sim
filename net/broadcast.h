#ifndef BROADCAST_H
#define BROADCAST_H

#include <export.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/asio.hpp>

#include <base/raw_data.h>
#include <base/gate.hpp>

namespace sim
{
    namespace net
    {


using namespace boost::asio;

class SIMLIB_API broadcast : public boost::noncopyable
{
    typedef std::pair<base::raw_data, ip::udp::endpoint> receive_data_pair;

    broadcast(unsigned int port);

public:
    ~broadcast();

    static boost::shared_ptr<broadcast> create(unsigned int port);

    void enable_message_mode(unsigned int messages_queue_length);

    bool get_message(base::raw_data& raw_data, ip::udp::endpoint* endpoint_sender_ptr = nullptr);

    template<typename T>
    bool get_message(T& val, ip::udp::endpoint* endpoint_sender_ptr = nullptr)
    {
        base::raw_data raw_data;
        if(get_message(raw_data, endpoint_sender_ptr)
        && raw_data.get_data(val))
        {
            return true;
        }

        return false;
    }

    void send_message(const base::raw_data& raw_data);

    template<typename T>
    void send_message(const T& msg)
    {
        base::raw_data raw_data;
        raw_data.set_data(msg);
        send_message(raw_data);
    }

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
    base::gate_interface<base::raw_data>*
                            m_gate_send;
    base::gate_interface<receive_data_pair>*
                            m_gate_receive;

    receive_data_pair       m_buffer;
    receive_data_pair       m_tmp_receive_data_pair;
};

typedef boost::shared_ptr<broadcast> broadcast_ptr;


    } // namespace net
} // namespace sim

#endif // BROADCAST_H
