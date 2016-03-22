#ifndef SIMBEACONMODE_H
#define SIMBEACONMODE_H

#include "net/broadcastsender.hpp"
#include "net/broadcastreceiver.hpp"
#include "beaconmessage.h"
#include "beaconmode.h"
#include <tools/tools.h>

#include <unordered_map>

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

typedef std::pair<unsigned char, beacon::data> beacon_data_pair;
typedef std::unordered_map<unsigned char, beacon::data> beacons_list;

///////////////////////////////////////////////////////////////////////////////

class mode_default : public sim::beacon::mode_abstract
{
public:
    mode_default(boost::shared_ptr< net::broadcast_sender<sim::beacon::message> > sender,
                 boost::shared_ptr< net::broadcast_receiver<sim::beacon::message> > receiver,
                 beacon::data beacon_data);

    bool run() override;

private:
    const unsigned int mc_time_duration;

    boost::shared_ptr<net::broadcast_sender<sim::beacon::message>>   m_sender;
    boost::shared_ptr<net::broadcast_receiver<sim::beacon::message>> m_receiver;

    sim::tool::timer m_timer;

    const beacon::data m_beacon_data;

    int m_last_response_message_mark;
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
    beacon::beacons_list    m_beacons_list;
    unsigned int m_last_response_message_mark;
};


    } // namespace beacon
} // namespace sim

#endif // SIMBEACONMODE_H
