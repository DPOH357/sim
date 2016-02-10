#ifndef MODEAUTHEN_H
#define MODEAUTHEN_H

#include"broadcastsender.hpp"
#include"broadcastreceiver.hpp"
#include"simbeaconmessage.h"
#include"simbeaconmodeabstract.h"
#include"simtools.h"

#include<set>

namespace sim
{
    namespace beacon
    {

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



#endif // MODEAUTHEN_H
