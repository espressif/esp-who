#include <freertos/FreeRTOS.h>
#include <vector>

template <typename T>
class RingBuf {
private:
    T *m_buffer;
    int m_capacity;
    int m_head;
    int m_count;

public:
    RingBuf(int capacity) : m_buffer(new T[capacity]), m_capacity(capacity), m_head(0), m_count(0) {}

    ~RingBuf() { delete[] m_buffer; }

    void push(const T &value)
    {
        if (m_count == m_capacity) {
            ESP_LOGE("RingBuf", "RingBuf is full.");
        }
        m_buffer[(m_head + m_count) % m_capacity] = value;
        m_count++;
    }

    T pop()
    {
        if (m_count == 0) {
            ESP_LOGE("RingBuf", "RingBuf is empty.");
        }
        T value = m_buffer[m_head];
        m_head = (m_head + 1) % m_capacity;
        m_count--;
        return value;
    }

    T &operator[](int index)
    {
        if (index < 0 || index >= m_count) {
            ESP_LOGE("RingBuf", "Index out of range.");
        }
        return m_buffer[(m_head + index) % m_capacity];
    }

    std::vector<T> range(int start, int end) const
    {
        if (start < 0 || end > m_count || start > end) {
            ESP_LOGE("RingBuf", "Invalid range.");
        }
        if (start == end) {
            return {};
        }
        int rel_start = (m_head + start) % m_capacity;
        int rel_end = (m_head + end) % m_capacity;
        std::vector<T> sub_buf;
        if (rel_start < rel_end) {
            sub_buf = std::vector<T>(m_buffer + rel_start, m_buffer + rel_end);
        } else {
            sub_buf = std::vector<T>(m_buffer + rel_start, m_buffer + m_count);
            std::vector<T> sub_buf2(m_buffer, m_buffer + rel_end);
            sub_buf.insert(sub_buf.end(), sub_buf2.begin(), sub_buf2.end());
        }
        return sub_buf;
    }

    T &front()
    {
        if (m_count == 0) {
            ESP_LOGE("RingBuf", "RingBuf is empty.");
        }
        return m_buffer[m_head];
    }

    T &back()
    {
        if (m_count == 0) {
            ESP_LOGE("RingBuf", "RingBuf is empty.");
        }
        return m_buffer[(m_head + m_count - 1) % m_capacity];
    }

    int capacity() const { return m_capacity; }

    int size() const { return m_count; }

    bool empty() const { return m_count == 0; }

    bool full() const { return m_count == m_capacity; }
};
