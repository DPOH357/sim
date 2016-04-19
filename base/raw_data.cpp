#include "raw_data.h"

#include <cstring>

sim::base::raw_data::raw_data()
    : m_data_ptr(nullptr)
    , m_data_size(0)
{

}

sim::base::raw_data::raw_data(size_t size)
    : m_data_size(size)
{
    m_data_ptr = new char[size];
    std::memset(m_data_ptr, 0, size);
}

sim::base::raw_data::raw_data(const void *data_ptr, size_t data_size)
{
    m_data_ptr = new char[data_size];
    std::memcpy(m_data_ptr, data_ptr, data_size);
    m_data_size = data_size;
}

sim::base::raw_data::raw_data(const sim::base::raw_data &data)
{
    m_data_size = data.m_data_size;
    m_data_ptr = new char[data.m_data_size];
    std::memcpy(m_data_ptr, data.m_data_ptr, data.m_data_size);
}

sim::base::raw_data::~raw_data()
{
    delete [] m_data_ptr;
}

void sim::base::raw_data::set(const void *data_ptr, size_t data_size)
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

size_t sim::base::raw_data::get_data_size() const
{
    return m_data_size;
}

bool sim::base::raw_data::get_data(void *data_ptr, size_t data_size) const
{
    if(m_data_size <= data_size)
    {
        std::memcpy(data_ptr, m_data_ptr, m_data_size);

        return true;
    }

    return false;
}

void sim::base::raw_data::operator=(const sim::base::raw_data &raw_data)
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
