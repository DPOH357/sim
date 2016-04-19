#include "raw_data_queue.h"

sim::base::raw_data_queue::data::data()
    : m_b_valid(false)
{

}

sim::base::raw_data_queue::data::data(size_t data_size)
    : m_b_valid(false)
    , m_raw_data(data_size)
{
}

void sim::base::raw_data_queue::data::set_data(const base::raw_data &raw_data)
{
    m_raw_data = raw_data;
    m_b_valid = true;
}

void sim::base::raw_data_queue::data::reset_data()
{
    m_b_valid = false;
}

bool sim::base::raw_data_queue::data::is_valid() const
{
    return m_b_valid;
}

bool sim::base::raw_data_queue::data::get_data(base::raw_data& raw_data)
{
    if(m_b_valid)
    {
        raw_data = m_raw_data;
    }

    return m_b_valid;
}

//----------------------------------------------------------------------------


sim::base::raw_data_queue::raw_data_queue(unsigned int length)
    : m_pos_back(0)
    , m_pos_front(0)
{
    m_list.reserve(length);
    for(unsigned int i = 0; i < length; ++i)
    {
        m_list.push_back(new raw_data_queue::data());
    }
}

sim::base::raw_data_queue::~raw_data_queue()
{
    for(auto data : m_list)
    {
        delete data;
    }
}

void sim::base::raw_data_queue::push(const base::raw_data &raw_data)
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

bool sim::base::raw_data_queue::front(base::raw_data &raw_data)
{
    boost::lock_guard< boost::mutex > locker(m_mutex);

    return m_list[m_pos_front]->get_data(raw_data);
}

bool sim::base::raw_data_queue::pop(base::raw_data &raw_data)
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

void sim::base::raw_data_queue::pop()
{
    boost::lock_guard< boost::mutex > locker(m_mutex);

    raw_data_queue::data* data( m_list[m_pos_front] );
    if(data->is_valid())
    {
        data->reset_data();

        ++m_pos_front;
        if(m_pos_front >= m_list.size())
        {
            m_pos_front = 0;
        }
    }
}

bool sim::base::raw_data_queue::is_empty()
{
    boost::lock_guard< boost::mutex > locker(m_mutex);
    return (m_pos_back == m_pos_front);
}
