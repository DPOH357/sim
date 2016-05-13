#include "userslistagent.h"

UsersListAgent::UsersListAgent(unsigned int port, QString name)
    : m_dTimeCheck(4000)
    , m_dTimeUserLeft(10000)
    , m_name(name)
    , m_timePrev(QTime::currentTime())
{
    m_broadcast = sim::net::broadcast::create(port);
    m_name.reserve(name_length);

    m_broadcast->send_message(sim::base::raw_data(m_name.toLocal8Bit().constData(), name_length));
}

void UsersListAgent::run()
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
        QString addressStr = QString::fromStdString(endpoint.address().to_string());

        if(name != m_name)
        {
            auto i =  m_usersList.find(name);
            if(i == m_usersList.end())
            {
                m_usersList.insert(name, UserData(timeCurrent, name, addressStr));

                emit send_userIn(addressStr, name);
            }
            else
            {
                i.value().timeCheck = timeCurrent;
            }
        }
    }

    for(auto i = m_usersList.begin(); i != m_usersList.end(); )
    {
        if(i.value().timeCheck.msecsTo(timeCurrent) > m_dTimeUserLeft)
        {
            emit send_userOut(i.value().addressStr, i.value().name);
            i = m_usersList.erase(i);
        }
        else
        {
            ++i;
        }
    }
}
