#ifndef SIMTOOLS_H
#define SIMTOOLS_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <list>

namespace sim
{
    namespace tool
    {

#define LOG_MESSAGE(x) std::cout << x << std::endl;

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

class raw_data
{
public:
    raw_data(const void* data_ptr, size_t data_size);
    raw_data(const raw_data& data);
    ~raw_data();

    void set(const void* data_ptr, size_t data_size);

    template<class T>
    void set(const T& data) { set((void*)data, sizeof(T)); }

    size_t get_data_size() const;
    void get_data(void* data_ptr) const;
    template<class T>
    void get_data(T& data) const { get_data((void*)data); }

private:
    void*   m_data_ptr;
    size_t  m_data_size;
};

///////////////////////////////////////////////////////////////////////////////

class raw_data_queue
{
public:
    raw_data_queue(unsigned int length);

    void push(const void* data_ptr, size_t data_size);
private:
    unsigned int                m_length;
    std::list<tool::raw_data>   m_list;
};


    } // namespace tool
} // namespace sim

#endif // SIMTOOLS_H
