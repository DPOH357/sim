#ifndef SIMNETGATE_H
#define SIMNETGATE_H

#include <queue>
#include <boost/thread.hpp>

#include <base/tools.h>

namespace sim
{
    namespace net
    {

template <class T>
class SIMLIB_API gate_interface
{
public:
    gate_interface() {}
    virtual ~gate_interface() {}

    void push(const T& val)
    {
        boost::lock_guard<boost::mutex> locker(m_mutex);
        _push(val);
    }

    bool pop(T& val)
    {
        boost::lock_guard<boost::mutex> locker(m_mutex);
        return _pop(val);
    }

    bool empty()
    {
        boost::lock_guard<boost::mutex> locker(m_mutex);
        return _empty();
    }

private:
    virtual void _push(const T& val) = 0;
    virtual bool _pop(T& val) = 0;
    virtual bool _empty() = 0;

private:
    boost::mutex        m_mutex;
};


template <class T>
class SIMLIB_API gate_message : public gate_interface<T>
{
public:
    gate_message(unsigned int queue_max_size, unsigned short repeat_count = 1)
        : m_repeat_count(repeat_count)
        , m_queue_max_size(queue_max_size)
        , m_repeat_counter(0)
    {
        if(m_repeat_count == 0)
            m_repeat_count = 1;
    }

    void _push(const T& val) override
    {
        if(m_queue.size() >= m_queue_max_size)
        {
            m_queue.pop();
            sim::base::log::message(sim::base::log::level::Debug, std::string("Queue is full."));
        }

        m_queue.push(val);
    }

    bool _pop(T& val) override
    {
        if(!m_queue.empty())
        {
            val = m_queue.front();

            m_repeat_counter++;
            if(m_repeat_counter >= m_repeat_count)
            {
                m_repeat_counter = 0;
                m_queue.pop();
            }

            return true;
        }

        return false;
    }

    bool _empty() override
    {
        return m_queue.empty();
    }

private:
    unsigned short  m_repeat_count;
    unsigned int    m_queue_max_size;
    std::queue<T>   m_queue;
    unsigned short  m_repeat_counter;
};

template <class T>
class SIMLIB_API gate_stream : public gate_interface<T>
{
public:
    void _push(const T& val) override
    {
        m_buffer = val;
    }

    bool _pop(net::T &val) override
    {
        val = m_buffer;

        return true;
    }

    bool _empty() override
    {
        return false;
    }

private:
    T m_buffer;
};


    } // namespace net
} // namespace sim

#endif // SIMNETGATE_H

