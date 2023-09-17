#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <gsl/gsl>

struct PowerSwrData
{
    float power;
    float swr;
};


template <typename T>
class PowerSwrSeries final
{
public:
    explicit PowerSwrSeries(std::size_t t_size = 0u);

    void resize(std::size_t t_size);
    std::size_t size() const noexcept { return m_size; }

    void reset() noexcept;
    void fill(T value) noexcept;

    std::size_t readCount() const noexcept { return m_bytesForRead; };

    bool readyRead() const noexcept { return m_readyRead; }

    void push(const T &item) noexcept;
    bool write(gsl::span<T> src) noexcept;
    bool readAll(gsl::span<T> dst) noexcept;

private:
    std::size_t m_size         {0};
    std::size_t m_writePtr     {0};
    std::size_t m_bytesForRead {0};
    std::vector<T> m_buffer;
    bool m_readyRead {false};
};

template <typename T>
PowerSwrSeries<T>::PowerSwrSeries(std::size_t t_size) :
  m_size(t_size)
{
    m_buffer.resize(t_size);
    m_size = m_buffer.size();
}

template <typename T>
void PowerSwrSeries<T>::resize(std::size_t t_size)
{
    if (m_size == t_size)
        return;

    m_buffer.resize(t_size);

    m_size          = t_size;
    m_writePtr      = 0;
    m_bytesForRead  = 0;
    m_readyRead     = false;
}

template <typename T>
void PowerSwrSeries<T>::reset() noexcept
{
    m_writePtr     = 0;
    m_bytesForRead = 0;
    m_readyRead    = false;
}

template<typename T>
void PowerSwrSeries<T>::fill (T value) noexcept
{
	std::fill(m_buffer.begin(), m_buffer.end(), value);

	m_writePtr     = 0;
	m_bytesForRead = m_size;
	m_readyRead    = true;
}

template <typename T>
bool PowerSwrSeries<T>::readAll(gsl::span<T> dst) noexcept
{
    if ((dst.size() > m_size) || (m_bytesForRead < dst.size()))
        return false;

    const auto t_dstSize = dst.size();
    auto t_begin = m_buffer.begin();
    if (dst.size() < m_size)
        t_begin += (m_writePtr + (m_size - dst.size())) % m_size;
    else
        t_begin += m_writePtr;
    const auto t_distance = std::distance(t_begin, m_buffer.end());

    if (t_dstSize > t_distance) {
        std::copy(t_begin, m_buffer.end(), dst.begin());
        std::copy(m_buffer.begin(), m_buffer.begin() + (t_dstSize - t_distance), dst.begin() + t_distance);
    }
    else {
        std::copy(t_begin, t_begin + t_dstSize, dst.begin());
    }

    return true;
}

template <typename T>
void PowerSwrSeries<T>::push(const T &item) noexcept
{
    if (m_size == 0u)
        return;

    m_buffer.at(m_writePtr) = item;
    m_writePtr = (++m_writePtr) % m_size;
    m_bytesForRead = std::min<std::size_t>(++m_bytesForRead, m_size);
    m_readyRead    = m_bytesForRead == m_size;
}

template <typename T>
bool PowerSwrSeries<T>::write(gsl::span<T> src) noexcept
{
    if (m_size == 0u)
        return false;

    if (static_cast<std::size_t>(src.size()) >= m_buffer.size()) {
        std::copy(src.begin() + (src.size() - static_cast<int>(m_buffer.size())), src.end(), m_buffer.begin());
        m_writePtr = 0;
    }
    else {
        const auto t_srcSize = src.size();
        auto t_begin = m_buffer.begin(); t_begin += m_writePtr;
        const auto t_distance = std::distance(t_begin, m_buffer.end());

        if (t_srcSize > t_distance) {
            std::copy(src.begin(), src.begin() + t_distance, t_begin);
            std::copy(src.begin() + t_distance, src.end(), m_buffer.begin());
        }
        else {
            std::copy(src.begin(), src.end(), t_begin);
        }

        m_writePtr = (m_writePtr + t_srcSize) % m_size;
    }

    m_bytesForRead = std::min<std::size_t>(m_bytesForRead + src.size(), m_size);
    m_readyRead    = m_bytesForRead == m_size;

    return true;
}

