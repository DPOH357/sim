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

sim::tool::raw_data::raw_data()
    : m_data_ptr(nullptr)
    , m_data_size(0)
{

}

sim::tool::raw_data::raw_data(size_t size)
    : m_data_size(size)
{
    m_data_ptr = new char[size];
    std::memset(m_data_ptr, 0, size);
}

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
        if(m_data_ptr)
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

bool sim::tool::raw_data::get_data(void *data_ptr, size_t data_size) const
{
    if(m_data_size <= data_size)
    {
        std::memcpy(data_ptr, m_data_ptr, m_data_size);

        return true;
    }

    return false;
}

void sim::tool::raw_data::operator=(const sim::tool::raw_data &raw_data)
{
    if(m_data_size < raw_data.m_data_size)
    {
        if(m_data_ptr)
            delete [] m_data_ptr;

        m_data_ptr = new char[raw_data.m_data_size];
    }
    m_data_size = raw_data.m_data_size;
    std::memcpy(m_data_ptr, raw_data.m_data_ptr, m_data_size);
}

///////////////////////////////////////////////////////////////////////////////

sim::tool::raw_data_queue::data::data()
    : m_b_valid(false)
{

}

sim::tool::raw_data_queue::data::data(size_t data_size)
    : m_b_valid(false)
    , m_raw_data(data_size)
{
}

void sim::tool::raw_data_queue::data::set_data(const tool::raw_data &raw_data)
{
    m_raw_data = raw_data;
    m_b_valid = true;
}

void sim::tool::raw_data_queue::data::reset_data()
{
    m_b_valid = false;
}

bool sim::tool::raw_data_queue::data::is_valid() const
{
    return m_b_valid;
}

bool sim::tool::raw_data_queue::data::get_data(tool::raw_data& raw_data)
{
    if(m_b_valid)
    {
        raw_data = m_raw_data;
    }

    return m_b_valid;
}

//----------------------------------------------------------------------------


sim::tool::raw_data_queue::raw_data_queue(unsigned int length)
    : m_pos_back(0)
    , m_pos_front(0)
{
    m_list.reserve(length);
    for(unsigned int i = 0; i < length; ++i)
    {
        m_list.push_back(new raw_data_queue::data());
    }
}

sim::tool::raw_data_queue::~raw_data_queue()
{
    for(auto data : m_list)
    {
        delete data;
    }
}

void sim::tool::raw_data_queue::push(const raw_data &raw_data)
{
    boost::lock_guard< boost::mutex > locker(m_mutex);

    m_list[m_pos_back]->set_data(raw_data);

    ++m_pos_back;
    if(m_pos_back >= m_list.size())
    {
        m_pos_back = 0;
    }

    if(m_pos_back == m_pos_front)
    {
        ++m_pos_front;
    }
}

bool sim::tool::raw_data_queue::front(tool::raw_data &raw_data)
{
    boost::lock_guard< boost::mutex > locker(m_mutex);

    return m_list[m_pos_front]->get_data(raw_data);
}

bool sim::tool::raw_data_queue::pop(sim::tool::raw_data &raw_data)
{
    boost::lock_guard< boost::mutex > locker(m_mutex);

    raw_data_queue::data* data( m_list[m_pos_front] );
    if(data->get_data(raw_data))
    {
        data->reset_data();

        ++m_pos_front;
        if(m_pos_front >= m_list.size())
        {
            m_pos_front = 0;
        }

        return true;
    }

    return false;
}

bool sim::tool::raw_data_queue::is_empty()
{
    boost::lock_guard< boost::mutex > locker(m_mutex);
    return (m_pos_back == m_pos_front);
}
