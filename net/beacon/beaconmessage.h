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

struct data
{
    enum { name_size = 64 };

    unsigned char id;
    char name[name_size];
    boost::asio::ip::address address;

    data()
        : id(0)
    {
        memset(name, 0, name_size);
    }

    data(unsigned char _id, char _name[], boost::asio::ip::address _address)
        : id(_id)
        , address(_address)
    {
        strcpy_s(name, name_size, _name);
    }

    data(const beacon::data& data)
    {
        id = data.id;
        address = data.address;
        strcpy_s(name, name_size, data.name);
    }
};

struct message
{
    int  mark;
    beacon::message_type message_type;
    beacon::data data;

    message()
        : mark(0)
        , message_type(beacon::message_type::Invalid)
    {

    }

    message(int _mark)
        : mark(_mark)
        , message_type(message_type::Request)
    {

    }

    message(int _mark, const beacon::data& _data)
        : mark(_mark)
        , message_type(message_type::Response)
        , data(_data)
    {

    }
};


    } // namespace beacon
} // namespace sim

#endif // BEACON_MESSAGE_H
