#ifndef SIMBEACONMODEDEFAULT_H
#define SIMBEACONMODEDEFAULT_H

#include"broadcastsender.hpp"
#include"broadcastreceiver.hpp"
#include"simbeaconmessage.h"
#include"simbeaconmodeabstract.h"
#include"simtools.h"

namespace sim
{
    namespace beacon
    {

class mode_default : public sim::beacon::mode_abstract
{
public:
    mode_default(boost::shared_ptr<net::broadcast_sender<sim::beacon::message>> sender,
                 boost::shared_ptr<net::broadcast_receiver<sim::beacon::message>> receiver,
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


    } // namespace beacon
} // namespace sim


#endif // SIMBEACONMODEDEFAULT_H
