#ifndef BEACON_MESSAGE_H
#define BEACON_MESSAGE_H

#include <boost/asio.hpp>

namespace sim
{
    namespace net
    {

enum class beacon_message_type
{
    Invalid,
    Request,
    Response
};

struct beacon_message_data
{
    enum { name_size = 64 };

    unsigned char id;
    char name[name_size];
    char address[16];

    beacon_message_data()
        : id(0)
    {
        memset(name, 0, sizeof(name));
        memset(address, 0, sizeof(address));
    }

    beacon_message_data(unsigned char _id, char _name[], char _address[])
        : id(_id)
    {
        strncpy(name, _name, sizeof(name));
        strncpy(address, _address, sizeof(address));
    }

    beacon_message_data(const beacon_message_data& data)
    {
        id = data.id;
        strncpy(name, data.name, sizeof(name));
        strncpy(address, data.address, sizeof(address));
    }
};

struct beacon_message
{
    unsigned int  mark;
    beacon_message_type message_type;
    beacon_message_data data;

    beacon_message()
        : mark(0)
        , message_type(net::beacon_message_type::Invalid)
    {

    }

    beacon_message(unsigned int _mark)
        : mark(_mark)
        , message_type(net::beacon_message_type::Request)
    {

    }

    beacon_message(unsigned int _mark, const net::beacon_message_data& _data)
        : mark(_mark)
        , message_type(net::beacon_message_type::Response)
        , data(_data)
    {

    }
};


    } // namespace net
} // namespace sim

#endif // BEACON_MESSAGE_H
