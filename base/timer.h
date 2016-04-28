#ifndef TIMER_H
#define TIMER_H

#include <export.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace sim
{
    namespace base
    {


class SIMLIB_API timer
{
public:
    timer();
    void start(unsigned int ms);
    bool is_cutoff() const;

private:
    boost::posix_time::time_duration    m_time_duration;
    boost::posix_time::ptime            m_time_start;
};


    } // namespace base
} // namespace sim

#endif // TIMER_H
