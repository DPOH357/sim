#ifndef MULTICAST_SERVER_H
#define MULTICAST_SERVER_H

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

class SIMLIB_API multicast_server : public boost::noncopyable
{
    multicast_server(const std::string& multicast_address_str, unsigned short port);

public:
    ~multicast_server();

    static boost::shared_ptr<multicast_server> create(const std::string& multicast_address_str, unsigned short port);

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

    void do_run();

    void do_send();

    void handler_send( const boost::system::error_code &error_code
                     , std::size_t sended_bytes);

private:
    boost::atomic<bool>     m_b_valid;
    io_service              m_io_service;
    boost::thread*          m_thread;
    ip::udp::endpoint       m_endpoint;
    ip::udp::socket         m_socket;

    base::gate_interface<base::raw_data>*
                            m_gate_send;

    base::raw_data          m_buffer;
};

typedef boost::shared_ptr<multicast_server> multicast_server_ptr;


    } // namespace net
} // namespace sim

#endif // MULTICAST_SERVER_H
