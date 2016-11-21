#ifndef GATE_H
#define GATE_H

#include <log/log.h>
#include <boost/thread.hpp>
#include <queue>

namespace sim
{
    namespace base
    {


template <class T>
class gate_interface
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

    bool empty() const
    {
        boost::lock_guard<boost::mutex> locker(m_mutex);
        return _empty();
    }

private:
    virtual void _push(const T& val) = 0;
    virtual bool _pop(T& val) = 0;
    virtual bool _empty() const = 0;

private:
    mutable boost::mutex        m_mutex;
};


template <class T>
class gate_message : public gate_interface<T>
{
public:
    gate_message(unsigned int queue_max_length)
        : m_queue_max_length(queue_max_length)
    {
    }

    void _push(const T& val) override
    {
        if(m_queue.size() >= m_queue_max_length)
        {
            m_queue.pop();
            log::message(log::level::Debug, std::string("Queue is full."));
        }

        m_queue.push(val);
    }

    bool _pop(T& val) override
    {
        if(!m_queue.empty())
        {
            val = m_queue.front();
            m_queue.pop();

            return true;
        }

        return false;
    }

    bool _empty() const override
    {
        return m_queue.empty();
    }

private:
    unsigned int    m_queue_max_length;
    std::queue<T>   m_queue;
};

template <class T>
class gate_stream : public gate_interface<T>
{
public:
    gate_stream()
        : m_b_valid(false)
    {

    }

    void _push(const T& val) override
    {
        m_buffer = val;
        m_b_valid = true;
    }

    bool _pop(T &val) override
    {
        if(m_b_valid)
        {
            val = m_buffer;
            m_b_valid = false;

            return true;
        }

        return false;
    }

    bool _empty() const override
    {
        return !m_b_valid;
    }

private:
    bool m_b_valid;
    T m_buffer;
};


    } // namespace base
} // namespace sim

#endif // GATE_H

