#include "SwrFilter.h"
#include <cmath>

SwrFilter::SwrFilter() noexcept
{
    init();
}

void SwrFilter::setTimeInterval(float ms) noexcept
{
    m_sampleRate = 1000.0f/ms;
    init();
}

void SwrFilter::setTimeConstant(float ms) noexcept
{
    m_tau = ms;
    init();
}

void SwrFilter::reset() noexcept
{
    m_swr.reset();
}

float SwrFilter::process(float swr) noexcept
{
    if (m_swr) {
        m_swr = filter(*m_swr, swr);
        return *m_swr;
    }

    m_swr = swr;
    return swr;
}

void SwrFilter::init() noexcept
{
    m_alpha = std::exp(-1000/(m_tau*m_sampleRate));
}

float SwrFilter::filter(float prev, float current) const noexcept
{
    return prev*m_alpha + current*(1.0f - m_alpha);
}




SwrImpulseFilter::SwrImpulseFilter() noexcept
{
    init();
}

void SwrImpulseFilter::setTimeInterval(float ms) noexcept
{
    m_sampleRate = 1000.0f/ms;
    init();
}

void SwrImpulseFilter::setTimeConstant(float ms) noexcept
{
    m_tau = ms;
    init();
}

void SwrImpulseFilter::reset() noexcept
{
    m_swr.reset();
}

float SwrImpulseFilter::process(float swr) noexcept
{
    if (m_swr) {
        m_swr = filter(swr);
        return *m_swr;
    }

    m_in  = swr;
    m_out = 0.0f;
    m_swr = swr;
    filter(swr);
    return m_out;
}

void SwrImpulseFilter::init() noexcept
{
    m_alpha = std::exp(-1000/(m_tau*m_sampleRate));
}

float SwrImpulseFilter::filter(float value) noexcept
{
    m_out = value - m_in + m_alpha*m_out;
    m_in  = value;
    return std::max(0.0f, -m_out);
}
