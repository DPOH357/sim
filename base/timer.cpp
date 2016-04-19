#include "timer.h"

sim::base::timer::timer()
{

}

void sim::base::timer::start(unsigned int ms)
{
    m_time_start = boost::posix_time::second_clock::local_time();
    m_time_duration = boost::posix_time::milliseconds(ms);
}

bool sim::base::timer::is_cutoff() const
{
    boost::posix_time::time_duration time_duration =
            boost::posix_time::second_clock::local_time() - m_time_start;

    return time_duration > m_time_duration;
}
