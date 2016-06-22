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

struct beacon_data
{
    unsigned char id;
    std::string name;
    std::string address_str;
    unsigned short port;

    beacon_data() : id(0), port(0) { }
    beacon_data(const beacon_data& bd)
        : id(bd.id)
        , name(bd.name)
        , address_str(bd.address_str)
        , port(bd.port)
    {}
    beacon_data(
            unsigned char _id,
            const std::string& _name,
            const std::string& _address_str,
            unsigned short _port)
        : id(_id)
        , name(_name)
        , address_str(_address_str)
        , port(_port)
    {}
};

struct beacon_message_data
{
    enum { name_size = 128 };

    unsigned char id;
    char name[name_size];

    beacon_message_data()
        : id(0)
    {
        memset(name, 0, sizeof(name));
    }

    beacon_message_data(unsigned char _id, char _name[])
        : id(_id)
    {
        strncpy(name, _name, sizeof(name));
    }

    beacon_message_data(const beacon_message_data& data)
    {
        id = data.id;
        strncpy(name, data.name, sizeof(name));
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
