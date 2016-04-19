#ifndef RAW_DATA_QUEUE_H
#define RAW_DATA_QUEUE_H

#include <boost/thread.hpp>
#include <vector>
#include "raw_data.h"

namespace sim
{
    namespace base
    {


class raw_data_queue
{
    struct data
    {
        data();
        data(size_t data_size);

        void set_data(const base::raw_data& raw_data);
        void reset_data();

        bool is_valid() const;

        bool get_data(base::raw_data& raw_data);

    private:
        bool            m_b_valid;
        base::raw_data  m_raw_data;
    };

public:
    raw_data_queue(unsigned int length);
    ~raw_data_queue();

    void push(const base::raw_data& raw_data);
    bool front(base::raw_data& raw_data);
    bool pop(base::raw_data& raw_data);
    void pop();

    bool is_empty();

private:
    boost::mutex                m_mutex;
    unsigned int                m_pos_back;
    unsigned int                m_pos_front;

    std::vector<raw_data_queue::data*>
                                m_list;
};


    } // namespace base
} // namespace sim

#endif // RAW_DATA_QUEUE_H
