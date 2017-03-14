#ifndef BROADCAST_RECEIVER_H
#define BROADCAST_RECEIVER_H

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

class SIMLIB_API broadcast_receiver : public boost::noncopyable
{
    typedef std::pair<base::raw_data, ip::udp::endpoint> receive_data_pair;

    broadcast_receiver(unsigned short port);

public:
    ~broadcast_receiver();

    static boost::shared_ptr<broadcast_receiver> create(unsigned short port);

    void enable_message_mode(unsigned int messages_queue_length);

    bool get_message(base::raw_data& raw_data, std::string* address_str_ptr = nullptr, unsigned short* port_ptr = nullptr);

private:
    void run();

    void do_receive();

    void handler_receive( const boost::system::error_code &error_code
                        , std::size_t receive_bytes);

private:
    boost::atomic<bool>     m_b_valid;
    io_service              m_io_service;
    boost::thread*          m_thread;
    ip::udp::socket         m_socket;
    ip::udp::endpoint       m_endpoint;

    base::gate_interface<receive_data_pair>*
                            m_gate_receive;

    receive_data_pair       m_buffer;
    receive_data_pair       m_tmp_receive_data_pair;
};

typedef boost::shared_ptr<broadcast_receiver> broadcast_receiver_ptr;


    } // namespace net
} // namespace sim

#endif // BROADCAST_RECEIVER_H
