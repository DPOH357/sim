#ifndef BEACON_MESSAGE_H
#define BEACON_MESSAGE_H

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
    unsigned char param;

    message()
        : mark(0)
        , message_type(beacon::message_type::Invalid)
        , param(0)
    {

    }

    message(unsigned int _mark, beacon::message_type _message_type, unsigned char _param = 0)
        : mark(_mark)
        , message_type(_message_type)
        , param(_param)
    {

    }
};


    } // namespace beacon
} // namespace sim

#endif // BEACON_MESSAGE_H
