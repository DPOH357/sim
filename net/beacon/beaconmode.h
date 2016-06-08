#ifndef SIMBEACONMODE_H
#define SIMBEACONMODE_H

#include <net/broadcast.h>
#include <net/beacon/beaconmessage.h>
#include <net/beacon/beaconmode.h>
#include <base/timer.h>

#include <unordered_map>

namespace sim
{
    namespace net
    {


class beacon_mode_abstract
{
public:
    beacon_mode_abstract() {}
    virtual ~beacon_mode_abstract() {}

    virtual bool run() = 0;
};

///////////////////////////////////////////////////////////////////////////////

typedef std::pair<unsigned char, net::beacon_message_data> beacon_data_pair;
typedef std::unordered_map<unsigned char, net::beacon_message_data> beacons_list;

///////////////////////////////////////////////////////////////////////////////

class beacon_mode_default : public net::beacon_mode_abstract
{
public:
    beacon_mode_default(boost::shared_ptr<sim::net::broadcast> broadcast,
                 net::beacon_message_data beacon_data);

    bool run() override;

private:
    const unsigned int                  mc_time_duration;

    boost::shared_ptr<net::broadcast>   m_broadcast;

    sim::base::timer                    m_timer;

    const beacon_message_data           m_beacon_data;

    unsigned int                        m_last_response_message_mark;
};

///////////////////////////////////////////////////////////////////////////////

class beacon_mode_authen : public net::beacon_mode_abstract
{
public:
    beacon_mode_authen(boost::shared_ptr<net::broadcast> broadcast);

    bool run() override;

    unsigned int get_free_id() const;

private:
    const unsigned int      mc_timer_duration;
    const unsigned int      mc_timer_main_duration;
    boost::shared_ptr<net::broadcast> m_broadcast;

    sim::base::timer        m_timer;
    sim::base::timer        m_timer_main;
    net::beacons_list       m_beacons_list;
    unsigned int            m_last_response_message_mark;
};


    } // namespace net
} // namespace sim

#endif // SIMBEACONMODE_H
