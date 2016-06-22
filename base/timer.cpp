#include "timer.h"

sim::base::timer::timer(bool b_nonstop)
    : m_b_nonstop(b_nonstop)
{

}

void sim::base::timer::start(unsigned int ms)
{
    m_time_start = boost::posix_time::second_clock::local_time();
    m_time_duration = boost::posix_time::milliseconds(ms);
}

void sim::base::timer::set_nonstop(bool b_nonstop)
{
    m_b_nonstop = b_nonstop;
}

bool sim::base::timer::is_cutoff() const
{
    auto time = boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration time_duration =
            time - m_time_start;

    bool rezult = time_duration > m_time_duration;
    if(rezult && m_b_nonstop)
    {
        m_time_start = time;
    }
    return rezult;
}
