#ifndef BEACON_H
#define BEACON_H

#include "net/broadcastsender.hpp"
#include "net/broadcastreceiver.hpp"
#include "beaconmessage.h"
#include "beaconmode.h"


namespace sim
{
    namespace beacon
    {


typedef boost::shared_ptr< net::broadcast_sender<sim::beacon::message> >    broadcast_sender;
typedef boost::shared_ptr< net::broadcast_receiver<sim::beacon::message> >  broadcast_receiver;

class beacon
{
public:
    beacon(boost::asio::ip::address address, unsigned int port);
    ~beacon();

    void run();

    unsigned int get_id() const { return m_id; }

private:
    enum class mode
    {
        Authentication,
        Main
    };

private:
    broadcast_sender            m_sender;
    broadcast_receiver          m_receiver;

    sim::beacon::mode_abstract* m_mode;

    unsigned int m_id;

    const boost::asio::ip::address m_address;
};


    } // namespace beacon
} // namespace sim


#endif // BEACON_H
