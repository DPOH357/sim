#ifndef MULTICAST_CLIENT_H
#define MULTICAST_CLIENT_H

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


using namespace boost;
using namespace boost::asio;

class SIMLIB_API multicast_client : public boost::noncopyable
{
    typedef std::pair<base::raw_data, ip::udp::endpoint> receive_data_pair;

    multicast_client(const std::string& multicast_address_str, unsigned int multicast_port);

public:
    ~multicast_client();

    static boost::shared_ptr<multicast_client> create(const std::string& multicast_address_str, unsigned int multicast_port);

    bool get_message(base::raw_data& raw_data, ip::udp::endpoint* endpoint_ptr = nullptr);

    template <typename T>
    bool get_message(T& message, ip::address *address_ptr = nullptr, unsigned short* port_ptr = nullptr)
    {
        base::raw_data raw_data;
        if(get_message(raw_data, address_ptr, port_ptr))
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

private:
    void run();

    void do_run();

    void do_receive();

    void handler_receive( const boost::system::error_code &error_code
                        , std::size_t receive_bytes);

private:
    boost::atomic<bool>     m_b_valid;
    io_service              m_io_service;
    boost::thread*          m_thread;
    ip::udp::socket         m_socket;

    base::gate_interface<receive_data_pair>*
                            m_gate_receive;

    receive_data_pair       m_buffer_pair;
    receive_data_pair       m_tmp_receive_data_pair;
};

typedef boost::shared_ptr<multicast_client> multicast_client_ptr;


    } // namespace net
} // namespace sim

#endif // MULTICAST_CLIENT_H
