#ifndef RAW_DATA_H
#define RAW_DATA_H

namespace sim
{
    namespace base
    {


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

        T* data_ptr = reinterpret_cast<T*>(m_data_ptr);
        *data_ptr = data;
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

    void operator=(const base::raw_data& raw_data);

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


    } // namespace base
} // namespace sim

#endif // RAW_DATA_H
