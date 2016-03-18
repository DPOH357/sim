#ifndef SIMBEACONMODE_H
#define SIMBEACONMODE_H

#include "net/broadcastsender.hpp"
#include "net/broadcastreceiver.hpp"
#include "beaconmessage.h"
#include "beaconmode.h"
#include <tools/tools.h>

#include <set>

namespace sim
{
    namespace beacon
    {


class mode_abstract
{
public:
    mode_abstract() {}
    virtual ~mode_abstract() {}

    virtual bool run() = 0;
};

///////////////////////////////////////////////////////////////////////////////

class mode_default : public sim::beacon::mode_abstract
{
public:
    mode_default(boost::shared_ptr< net::broadcast_sender<sim::beacon::message> > sender,
                 boost::shared_ptr< net::broadcast_receiver<sim::beacon::message> > receiver,
                 unsigned char id);

    bool run() override;

private:
    const unsigned int mc_time_duration;

    boost::shared_ptr<net::broadcast_sender<sim::beacon::message>>   m_sender;
    boost::shared_ptr<net::broadcast_receiver<sim::beacon::message>> m_receiver;

    sim::tool::timer m_timer;

    unsigned char m_id;

    unsigned int m_last_response_message_mark;
};

///////////////////////////////////////////////////////////////////////////////

class mode_authen : public sim::beacon::mode_abstract
{
public:
    mode_authen(boost::shared_ptr<net::broadcast_sender<sim::beacon::message>> sender,
                boost::shared_ptr<net::broadcast_receiver<sim::beacon::message>> receiver);

    bool run() override;

    unsigned char get_free_id() const;

private:
    const unsigned int      mc_timer_duration;
    const unsigned int      mc_timer_main_duration;
    boost::shared_ptr<net::broadcast_sender<sim::beacon::message>>   m_sender;
    boost::shared_ptr<net::broadcast_receiver<sim::beacon::message>> m_receiver;

    sim::tool::timer        m_timer;
    sim::tool::timer        m_timer_main;
    std::set<unsigned char> m_busy_id_list;
    unsigned int m_last_response_message_mark;
};


    } // namespace beacon
} // namespace sim

#endif // SIMBEACONMODE_H
