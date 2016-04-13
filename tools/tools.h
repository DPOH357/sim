#ifndef SIMTOOLS_H
#define SIMTOOLS_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <vector>

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
    raw_data();
    raw_data(size_t size);
    raw_data(const void* data_ptr, size_t data_size);
    raw_data(const raw_data& data);

    template <class T>
    raw_data(const T& data)
        : m_data_size(sizeof(T))
    {
        m_data_ptr = new char[sizeof(T)];
        std::memcpy(m_data_ptr, &data, sizeof(T));
    }

    ~raw_data();

    void        set(const void* data_ptr, size_t data_size);

    template<class T>
    void        set(const T& data) { set((void*)data, sizeof(T)); }

    size_t      get_data_size() const;
    bool        get_data(void* data_ptr, size_t data_size) const;

    template<class T>
    bool        get_data(T& data) const { return get_data((void*)&data, sizeof(T)); }

    void* get_data_ptr() const { return m_data_ptr; }

    void operator=(const tool::raw_data& raw_data);

    template <class T>
    void operator=(const T& data)
    {
        if(m_data_size < sizeof(T))
        {
            if(m_data_ptr)
                delete [] m_data_ptr;

            m_data_ptr = new char[sizeof(T)];
        }
        m_data_size = sizeof(T);
        std::memcpy(m_data_ptr, &data, m_data_size);
    }

private:
    void*   m_data_ptr;
    size_t  m_data_size;
};

///////////////////////////////////////////////////////////////////////////////

class raw_data_queue
{
    struct data
    {
        data();
        data(size_t data_size);

        void set_data(const tool::raw_data& raw_data);
        void reset_data();

        bool is_valid() const;

        bool get_data(tool::raw_data& raw_data);

    private:
        bool            m_b_valid;
        tool::raw_data  m_raw_data;
    };

public:
    raw_data_queue(unsigned int length);
    ~raw_data_queue();

    void push(const tool::raw_data& raw_data);
    bool front(tool::raw_data& raw_data);
    bool pop(tool::raw_data& raw_data);

    bool is_empty();

private:
    boost::mutex                m_mutex;
    unsigned int                m_pos_back;
    unsigned int                m_pos_front;

    std::vector<raw_data_queue::data*>
                                m_list;
};


    } // namespace tool
} // namespace sim

#endif // SIMTOOLS_H
