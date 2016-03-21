#ifndef BEACON_MESSAGE_H
#define BEACON_MESSAGE_H

#include <boost/asio.hpp>

namespace sim
{
    namespace beacon
    {

enum class message_type
{
    Invalid,
    Request,
    Response
};

struct message
{
    unsigned int  mark;
    beacon::message_type message_type;
    unsigned char id;
    boost::asio::ip::address address;

    message()
        : mark(0)
        , message_type(beacon::message_type::Invalid)
        , id(0)
    {

    }

    message(unsigned int _mark, beacon::message_type _message_type, unsigned char _param = 0, boost::asio::ip::address _address = boost::asio::ip::address())
        : mark(_mark)
        , message_type(_message_type)
        , id(_param)
        , address(_address)
    {

    }
};


    } // namespace beacon
} // namespace sim

#endif // BEACON_MESSAGE_H
