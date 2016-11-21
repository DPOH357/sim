#ifndef SIMBEACONMODE_H
#define SIMBEACONMODE_H

#include <net/broadcast.h>
#include <net/beacon/beaconmessage.hpp>
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

typedef std::pair<unsigned char, net::beacon_data> beacon_data_pair;
typedef std::unordered_map<unsigned char, net::beacon_data> beacons_list;

///////////////////////////////////////////////////////////////////////////////

class beacon_mode_default : public net::beacon_mode_abstract
{
public:
    beacon_mode_default(
            boost::shared_ptr<sim::net::broadcast> broadcast,
            net::beacon_message_data beacon_data,
            const net::beacons_list& beacons_list);

    bool run() override;
    void get_beacons_list(net::beacons_list& beacons_list) const;

private:
    const unsigned int                  mc_time_duration;
    boost::shared_ptr<net::broadcast>   m_broadcast;
    sim::base::timer                    m_timer_responce;
    const beacon_message_data           m_beacon_data;
    unsigned int                        m_last_response_message_mark;
    net::beacons_list                   m_beacons_list;
};

///////////////////////////////////////////////////////////////////////////////

class beacon_mode_authen : public net::beacon_mode_abstract
{
public:
    beacon_mode_authen(boost::shared_ptr<net::broadcast> broadcast);

    bool run() override;

    unsigned int get_free_id() const;
    net::beacons_list  get_beacons_list() const;

private:
    const unsigned int      mc_timer_request_duration;
    const unsigned int      mc_timer_life_duration;
    net::broadcast_ptr      m_broadcast;

    sim::base::timer        m_timer_request;
    sim::base::timer        m_timer_life;
    net::beacons_list       m_beacons_list;
    unsigned int            m_last_response_message_mark;
};


    } // namespace net
} // namespace sim

#endif // SIMBEACONMODE_H
