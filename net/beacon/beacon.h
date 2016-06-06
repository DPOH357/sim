#ifndef NETBEACON_H
#define NETBEACON_H

#include <net/broadcast.h>
#include <string>
#include "beaconmode.h"


namespace sim
{
    namespace net
    {


class beacon
{
public:
    beacon(const std::string& name, const std::string& address_str, unsigned int port);
    ~beacon();

    void run();

    unsigned int get_id() const { return m_id; }

private:
    enum { beacon_name_max_length = 64 };
    boost::shared_ptr<net::broadcast>   m_broadcast;

    net::beacon_mode_abstract*          m_mode;

    unsigned int                        m_id;
    char                                m_name[beacon_name_max_length];
    char                                m_address_str[16];
};


    } // namespace net
} // namespace sim


#endif // NETBEACON_H
