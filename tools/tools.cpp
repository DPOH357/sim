#include "tools.h"

#include <stdlib.h>

int sim::tool::random(int rangeMin, int rangeMax)
{
    if(rangeMin > rangeMax)
    {
        int tmp = rangeMin;
        rangeMin = rangeMax;
        rangeMax = tmp;
    }
    return (rand() / (RAND_MAX + 1) \
        * (rangeMax - rangeMin) + rangeMin);
}

///////////////////////////////////////////////////////////////////////////////

sim::tool::timer::timer()
{

}

void sim::tool::timer::start(unsigned int ms)
{
    m_time_start = boost::posix_time::second_clock::local_time();
    m_time_duration = boost::posix_time::milliseconds(ms);
}

bool sim::tool::timer::is_cutoff() const
{
    boost::posix_time::time_duration time_duration =
            boost::posix_time::second_clock::local_time() - m_time_start;

    return time_duration > m_time_duration;
}
