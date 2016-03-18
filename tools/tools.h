#ifndef SIMTOOLS_H
#define SIMTOOLS_H

#include<boost/date_time/posix_time/posix_time.hpp>

namespace sim
{
    namespace tool
    {

int random(int rangeMin, int rangeMax);

///////////////////////////////////////////////////////////////////////////////

class timer
{
public:
    timer();
    void start(unsigned int ms);
    bool is_cutoff() const;

private:
    boost::posix_time::time_duration    m_time_duration;
    boost::posix_time::ptime            m_time_start;
};


    } // namespace tool
} // namespace sim

#endif // SIMTOOLS_H
