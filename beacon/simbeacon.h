#ifndef SIMBEACON_H
#define SIMBEACON_H

#include "net/broadcastsender.hpp"
#include "net/broadcastreceiver.hpp"
#include "beacon/simbeaconmessage.h"
#include "beacon/simbeaconmodeabstract.h"


namespace sim
{
    namespace beacon
    {


typedef boost::shared_ptr<net::broadcast_sender<sim::beacon::message>> broadcast_sender;
typedef boost::shared_ptr<net::broadcast_receiver<sim::beacon::message>> broadcast_receiver;

class beacon
{
public:
    beacon(unsigned int port);
    ~beacon();

    void run();

    unsigned int get_id() const { return m_id; }

private:
    enum class mode
    {
        Authentication,
        Main
    };

    void run_authentication();
    void run_main();

private:
    broadcast_sender m_sender;
    broadcast_receiver m_receiver;

    sim::beacon::mode_abstract* m_mode;

    unsigned int m_id;
};


    } // namespace beacon
} // namespace sim


#endif // SIMBEACON_H
