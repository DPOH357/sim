#ifndef RAW_DATA_H
#define RAW_DATA_H

#include <export.h>

#include <memory>

namespace sim
{
    namespace base
    {


class SIMLIB_API raw_data
{
public:
    raw_data();
    raw_data(size_t size);
    raw_data(const void* data_ptr, size_t data_size);
    raw_data(const raw_data& data);

    ~raw_data();

    void        reserve(size_t size);

    void        set_data(const void* data_ptr, size_t data_size);

    template<class T>
    void        set_data(const T& data) { set_data(&data, sizeof(T)); }

    size_t      get_data_size() const;
    bool        get_data(void* data_ptr, size_t data_size) const;

    size_t      get_reserved_size() const;

    template<class T>
    bool        get_data(T& data) const { return get_data((void*)&data, sizeof(T)); }

    void *get_data_ptr() const;

    void set_data_size(size_t data_size);

    void operator=(const base::raw_data& raw_data);

    template <class T>
    void operator=(const T& data)
    {
        if(m_data_size < sizeof(T))
        {
            if(m_memory_ptr)
                delete [] m_memory_ptr;

            m_memory_ptr = new char[sizeof(T)];
        }
        m_data_size = sizeof(T);
        memcpy(m_memory_ptr, &data, m_data_size);
    }

private:
    size_t  m_memory_size;
    char*   m_memory_ptr;
    size_t  m_data_size;
};


    } // namespace base
} // namespace sim

#endif // RAW_DATA_H
