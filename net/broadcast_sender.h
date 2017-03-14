#ifndef BROADCAST_SENDER_H
#define BROADCAST_SENDER_H

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

// !!! NOT TESTED
using namespace boost::asio;

class SIMLIB_API broadcast_sender : public boost::noncopyable
{
    broadcast_sender(unsigned short port);

public:
    ~broadcast_sender();

    static boost::shared_ptr<broadcast_sender> create(unsigned short port);

    void enable_message_mode(unsigned int messages_queue_length);

    void send_message(const base::raw_data& raw_data);

private:
    void run();

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

    base::raw_data          m_buffer;
};

typedef boost::shared_ptr<broadcast_sender> broadcast_sender_ptr;


    } // namespace net
} // namespace sim

#endif // BROADCAST_SENDER_H
