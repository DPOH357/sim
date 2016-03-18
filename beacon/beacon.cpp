#include"beacon.h"

#include"beaconmodeauthen.h"
#include"beaconmodedefault.h"

sim::beacon::beacon::beacon(unsigned int port)
    : m_mode(nullptr)
    , m_id(0)
{
    m_sender = net::broadcast_sender<sim::beacon::message>::create(port);
    m_receiver = net::broadcast_receiver<sim::beacon::message>::create(port);

    m_mode = new sim::beacon::mode_authen(m_sender, m_receiver);
}

sim::beacon::beacon::~beacon()
{
    delete m_mode;
}

void sim::beacon::beacon::run()
{
    if(!m_mode->run())
    {
        auto authentication_mode = dynamic_cast<sim::beacon::mode_authen*>(m_mode);
        if(authentication_mode)
        {
            m_id = authentication_mode->get_free_id();
            if(m_id)
            {
                delete m_mode;
                m_mode = new sim::beacon::mode_default(m_sender, m_receiver, m_id);
                LOG_MESSAGE("Start default mode");
            }
        }
        else
        {
            delete m_mode;
            m_mode = new sim::beacon::mode_authen(m_sender, m_receiver);
            LOG_MESSAGE("Start authentication mode");
        }
    }
}
