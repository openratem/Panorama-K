#ifndef RINGBUFFEREVENT_H
#define RINGBUFFEREVENT_H

#include <vector>
#include <atomic>
#include <stdint.h>
#include <QMutex>

namespace ee {

using namespace std;

template <typename T>
class RingBufferEvent
{
public:
    explicit RingBufferEvent(uint32_t t_size = 0);
    ~RingBufferEvent();

    void resize(uint32_t t_size);
    uint32_t size() const noexcept;

    void clear() noexcept;

    uint32_t readCount() const noexcept;
    uint32_t writeCount() const noexcept;

    bool read(vector<T> & dst) noexcept;
    bool write(vector<T> & src) noexcept;

    bool waitForReadyRead(uint32_t timeoutMs) noexcept;

private:
    RingBufferEvent(const RingBufferEvent &) = delete;
    RingBufferEvent &operator=(const RingBufferEvent &) = delete;

    inline void createEvent() noexcept;
    inline void setEvent() noexcept;
    inline void resetEvent() noexcept;
    inline bool waitEvent(uint32_t timeoutMs) noexcept;

private:
    uint32_t m_size;
    uint32_t m_readPtr;
    uint32_t m_writePtr;
    uint32_t m_mask;

    atomic<uint32_t> m_bytesForRead;
    atomic<uint32_t> m_bytesForWrite;

    vector<T> m_buffer;

    atomic<uint32_t> m_eventCnt;
    QMutex m_mutex;
};

template <typename T>
RingBufferEvent<T>::RingBufferEvent(uint32_t t_size) :
  m_size(t_size),
  m_readPtr(0),
  m_writePtr(0),
  m_mask(0),
  m_bytesForRead(0),
  m_bytesForWrite(0)
{
    m_buffer.resize(powToNext(t_size));
    m_size = m_buffer.size();
    m_mask = m_size - 1;
    m_bytesForWrite = m_size;

    createEvent();
}

template <typename T>
RingBufferEvent<T>::~RingBufferEvent()
{
    m_mutex.tryLock(1);
    m_mutex.unlock();
}

template <typename T>
void RingBufferEvent<T>::resize(uint32_t t_size)
{
    if (m_size == t_size)
        return;

    m_bytesForRead  = 0;
    m_bytesForWrite = 0;

    m_buffer.resize(powToNext(t_size));

    resetEvent();

    m_readPtr  = 0;
    m_writePtr = 0;

    m_size = m_buffer.size();
    m_mask = m_size - 1;
    m_bytesForWrite = m_size;
}

template <typename T>
uint32_t RingBufferEvent<T>::size() const noexcept
{
    return m_size;
}

template <typename T>
void RingBufferEvent<T>::clear() noexcept
{
    m_bytesForRead  = 0;
    m_bytesForWrite = 0;

    resetEvent();

    m_readPtr  = 0;
    m_writePtr = 0;

    m_bytesForWrite = m_size;
}

template <typename T>
uint32_t RingBufferEvent<T>::readCount() const noexcept
{
    return m_bytesForRead;
}

template <typename T>
uint32_t RingBufferEvent<T>::writeCount() const noexcept
{
    return m_bytesForWrite;
}

template <typename T>
bool RingBufferEvent<T>::read(vector<T> & dst) noexcept
{
    if ((dst.size() > m_bytesForRead) || (dst.size() == 0))
        return false;

    m_bytesForRead -= dst.size();
    if (m_bytesForRead == 0)
        resetEvent();

    for (T &data : dst)
        data = m_buffer[m_readPtr++&m_mask];
    m_bytesForWrite += dst.size();

    return true;
}

template <typename T>
bool RingBufferEvent<T>::write(vector<T> & src) noexcept
{
    if ((src.size() > m_bytesForWrite) || (src.size() == 0))
        return false;

    m_bytesForWrite -= src.size();
    for (T &data : src)
        m_buffer[m_writePtr++&m_mask] = data;
    m_bytesForRead += src.size();

    setEvent();
    return true;
}

template <typename T>
bool RingBufferEvent<T>::waitForReadyRead(uint32_t timeoutMs) noexcept
{
    if (m_bytesForRead > 0)
        return true;

    return waitEvent(timeoutMs);
}

template <typename T>
void RingBufferEvent<T>::createEvent() noexcept
{
    m_eventCnt = 0;
    m_mutex.lock();
}

template <typename T>
void RingBufferEvent<T>::setEvent() noexcept
{
    if (m_eventCnt == 0) {
        m_eventCnt = static_cast<int>(m_bytesForRead);
        m_mutex.unlock();
    }
    else {
        m_eventCnt = static_cast<int>(m_bytesForRead);
    }
}

template <typename T>
bool RingBufferEvent<T>::waitEvent(uint32_t timeoutMs) noexcept
{
    if (m_eventCnt > 0)
        return true;

    return m_mutex.tryLock(timeoutMs);
}

template <typename T>
void RingBufferEvent<T>::resetEvent() noexcept
{
    m_eventCnt = static_cast<int>(m_bytesForRead);
    if (m_eventCnt == 0)
        m_mutex.tryLock(1);
}

}

#endif // RINGBUFFEREVENT_H
