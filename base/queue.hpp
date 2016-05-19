#ifndef SIM_QUEUE_H
#define SIM_QUEUE_H

#include <boost/thread.hpp>
#include <vector>

namespace sim
{
    namespace base
    {

template <class T>
struct queue_data
{
    queue_data()
        : m_b_valid(false)
    {

    }

    void set_data(const T& data)
    {
        m_data = data;
        m_b_valid = true;
    }

    void reset_data()
    {
        m_b_valid = false;
    }

    bool is_valid() const
    {
        return m_b_valid;
    }

    bool get_data(T& data)
    {
        if(m_b_valid)
        {
            data = m_data;
        }

        return m_b_valid;
    }

private:
    bool    m_b_valid;
    T       m_data;
};

template <class T>
class queue
{
public:
    queue(unsigned int length)
        : m_pos_back(0)
        , m_pos_front(0)
    {
        boost::lock_guard< boost::mutex > locker(m_mutex);

        if(length < 2)
            length = 2;

        m_list.reserve(length);
        for(unsigned int i = 0; i < length; ++i)
        {
            m_list.push_back(queue_data<T>());
        }
    }

    ~queue()
    {
    }

    void push(const T& data)
    {
        boost::lock_guard< boost::mutex > locker(m_mutex);

        if(m_pos_back == m_pos_front
        && m_list[m_pos_front].is_valid())
        {
            ++m_pos_front;

            if(m_pos_front >= m_list.size())
            {
                m_pos_front = 0;
            }
        }

        m_list[m_pos_back].set_data(data);

        ++m_pos_back;
        if(m_pos_back >= m_list.size())
        {
            m_pos_back = 0;
        }
    }

    bool pop(T& data)
    {
        boost::lock_guard< boost::mutex > locker(m_mutex);

        queue_data<T>& qdata( m_list[m_pos_front] );
        if(qdata.get_data(data))
        {
            qdata.reset_data();

            ++m_pos_front;
            if(m_pos_front >= m_list.size())
            {
                m_pos_front = 0;
            }

            return true;
        }

        return false;
    }

    void pop()
    {
        boost::lock_guard< boost::mutex > locker(m_mutex);

        queue_data<T>& qdata( m_list[m_pos_front] );
        if(qdata->is_valid())
        {
            qdata.reset_data();

            ++m_pos_front;
            if(m_pos_front >= m_list.size())
            {
                m_pos_front = 0;
            }
        }
    }

    bool is_empty()
    {
        boost::lock_guard< boost::mutex > locker(m_mutex);
        return (m_pos_back == m_pos_front) && !m_list[m_pos_front].is_valid();
    }

    void clear()
    {
        boost::lock_guard< boost::mutex > locker(m_mutex);

        for(auto e : m_list)
        {
            e.reset_data();
        }

        m_pos_back = m_pos_front = 0;
    }

private:
    boost::mutex                m_mutex;
    unsigned int                m_pos_back;
    unsigned int                m_pos_front;

    std::vector< queue_data<T> >   m_list;
};


    } // namespace base
} // namespace sim

#endif // SIM_QUEUE_H
