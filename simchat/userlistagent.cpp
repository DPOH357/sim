#include "userlistagent.h"

UserListAgent::UserListAgent(unsigned int port, QString name)
    : m_dTimeCheck(1500)
    , m_dTimeUserLeft(5000)
    , m_name(name)
    , m_timePrev(QTime::currentTime())
{
    m_broadcast = sim::net::broadcast::create(port);
    m_name.reserve(name_length);
}

void UserListAgent::run()
{
    QTime timeCurrent = QTime::currentTime();
    if(m_timePrev.msecsTo( timeCurrent ) > m_dTimeCheck)
    {
        m_broadcast->send_message(sim::base::raw_data(m_name.toLocal8Bit().constData(), name_length));
        m_timePrev = timeCurrent;
    }

    sim::base::raw_data rawData;
    boost::asio::ip::udp::endpoint endpoint;
    while(m_broadcast->get_message(rawData, &endpoint))
    {
        char name_char[name_length];

        rawData.get_data((void*)name_char, name_length);
        QString name(name_char);

        auto i =  m_usersList.find(name);
        if(i == m_usersList.end())
        {
            m_usersList.insert(name, UserData(timeCurrent, endpoint));
            emit send_userIn(name);
        }
        else
        {
            i.value().timeCheck = timeCurrent;
        }
    }

    for(auto i = m_usersList.begin(); i != m_usersList.end(); )
    {
        if(i.value().timeCheck.msecsTo(timeCurrent) > m_dTimeUserLeft)
        {
            QString name = i.key();
            i = m_usersList.erase(i);
            emit send_userOut(name);
        }
        else
        {
            ++i;
        }
    }
}

