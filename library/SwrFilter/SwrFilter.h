#pragma once

#include <optional>

/**
 * \class SwrFilter
 * \brief Фильтр измерителя КСВ.
 * \details Показывает усреднённое значение КСВ.
 */
struct SwrFilter final
{
    /**
     * \brief Конструктор по умолчанию.
     */
    SwrFilter() noexcept;

    /**
     * \brief Установка периода измерений.
     * \param ms - период измерений, мс.
     */
    void setTimeInterval(float ms) noexcept;

    /**
     * \brief Установка постоянной времени.
     * \param ms - постоянная времени, мс.
     */
    void setTimeConstant(float ms) noexcept;

    /**
     * \brief Сброс в исходное состояние.
     */
    void reset() noexcept;

    /**
     * \brief Фильтрация КСВ.
     * \param swr - значение КСВ измерителя.
     * \return фильтрованное значение КСВ.
     */
    float process(float swr) noexcept;

private:
    void init() noexcept;
    float filter(float prev, float current) const noexcept;

private:
    std::optional<float> m_swr;
    float m_sampleRate {20.0f};
    float m_tau {20.0f};
    float m_alpha {1.0f};
};


/**
 * \class SwrImpulseFilter
 * \brief Фильтр для обнаружения резких изменений значения КСВ.
 * \details Показывает выбросы в значениях КСВ. Предназначен для выявления плохого контакта в ВЧ кабеле.
 */
struct SwrImpulseFilter final
{
    /**
     * \brief Конструктор по умолчанию.
     */
    SwrImpulseFilter() noexcept;

    /**
     * \brief Установка периода измерений.
     * \param ms - период измерений, мс.
     */
    void setTimeInterval(float ms) noexcept;

    /**
     * \brief Установка постоянной времени.
     * \param ms - постоянная времени, мс.
     */
    void setTimeConstant(float ms) noexcept;

    /**
     * \brief Сброс в исходное состояние.
     */
    void reset() noexcept;

    /**
     * \brief Фильтрация КСВ.
     * \param swr - значение КСВ измерителя.
     * \return фильтрованное значение КСВ.
     */
    float process(float swr) noexcept;

private:
    void init() noexcept;
    float filter(float value) noexcept;

private:
    std::optional<float> m_swr;
    float m_sampleRate {20.0f};
    float m_tau {20.0f};
    float m_alpha {1.0f};
    float m_in  {0.0f};
    float m_out {0.0f};
};
