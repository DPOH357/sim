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

///////////////////////////////////////////////////////////////////////////////

sim::tool::raw_data::raw_data(const void *data_ptr, size_t data_size)
{
    m_data_ptr = new char[data_size];
    std::memcpy(m_data_ptr, data_ptr, data_size);
    m_data_size = data_size;
}

sim::tool::raw_data::raw_data(const sim::tool::raw_data &data)
{
    m_data_size = data.m_data_size;
    m_data_ptr = new char[data.m_data_size];
    std::memcpy(m_data_ptr, data.m_data_ptr, data.m_data_size);
}

sim::tool::raw_data::~raw_data()
{
    delete [] m_data_ptr;
}

void sim::tool::raw_data::set(const void *data_ptr, size_t data_size)
{
    if(m_data_size < data_size)
    {
        delete [] m_data_ptr;
        m_data_ptr = new char[data_size];
    }

    m_data_size = data_size;
    std::memcpy(m_data_ptr, data_ptr, data_size);
}

size_t sim::tool::raw_data::get_data_size() const
{
    return m_data_size;
}

void sim::tool::raw_data::get_data(void *data_ptr) const
{
    std::memcpy(data_ptr, m_data_ptr, m_data_size);
}

///////////////////////////////////////////////////////////////////////////////

sim::tool::raw_data_queue::raw_data_queue(unsigned int length)
    : m_length(length)
{

}
