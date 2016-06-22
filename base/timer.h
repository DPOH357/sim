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
    timer(bool b_nonstop);
    void start(unsigned int ms);
    void set_nonstop(bool b_nonstop);
    bool is_cutoff() const;    
    bool is_nonstop() const;

private:
    boost::posix_time::time_duration    m_time_duration;
    mutable boost::posix_time::ptime    m_time_start;
    bool                                m_b_nonstop;
};


    } // namespace base
} // namespace sim

#endif // TIMER_H
