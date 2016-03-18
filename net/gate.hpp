#ifndef SIMNETGATE_H
#define SIMNETGATE_H

#include <queue>
#include <boost/thread.hpp>

namespace sim
{
    namespace net
    {

#define LOG_MESSAGE(x) std::cout << x << std::endl;


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

private:
    virtual void _push(const T& val) = 0;
    virtual bool _pop(T& val) = 0;

private:
    boost::mutex        m_mutex;
};


template <class T>
class gate_message : public gate_interface<T>
{
public:
    gate_message(unsigned short repeat_count, unsigned int queue_max_size)
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
            LOG_MESSAGE(std::string("Queue is full."));
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

private:
    unsigned short  m_repeat_count;
    unsigned int    m_queue_max_size;
    std::queue<T>   m_queue;
    unsigned short  m_repeat_counter;
};

template <class T>
class gate_stream : public gate_interface<T>
{
public:
    void _push(const T& val) override
    {
        if(m_queue.size() >= m_queue_max_size)
        {
            m_queue.pop();
            LOG_MESSAGE(std::string("Queue is full."));
        }

        m_queue.push(val);
    }

    bool _pop(net::T &val) override
    {
        if(!m_queue.empty())
        {
            val = m_queue.front();
            m_queue.pop();
            return true;
        }

        return false;
    }
};


    } // namespace net
} // namespace sim

#endif // SIMNETGATE_H

