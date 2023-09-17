#ifndef PANORAMAKCOMMON_H
#define PANORAMAKCOMMON_H

#include <stdint.h>

/**
 *  \enum ErrorCode
 *  \brief Перечисление типа ошибок.
 */
typedef enum {
    Success = 0, ///< Успешно.
    UnknownError = -1, ///< Неизвестная ошибка.
    DeviceIsNotOpened = -2, ///< Не удалось открыть устройство.
    InvalidDescriptor = -3, ///< Не валидный дескриптор.
    NullPointer = -4 ///< Нулевой указатель.
} ErrorCode;

/**
 * \enum ControlInterface
 * \brief Перечисление интерфейсов управления трансивером.
 */
typedef enum {
    UnknownProtocol = 0, ///< Неизвестный протокол
    Network = 1, ///< Управление по сети
    SerialPort = 2 ///< Управление через COM порт
} ControlType;

typedef void* ServiceDescriptor;

struct DeviceInfo {
    char* name;
    ControlType controlType;
    DeviceInfo* next;
};

/// Определение типа функции, которая вызывается при изменении частоты.
typedef void (*callbackFrequency)(int data, void* userData);

/// Определение типа функции, которая вызывается при изменении состояния соединения с устройством.
typedef void (*callbackConnectionStatus)(int data, void* userData);

/// Определение типа функции, которая вызывается при изменении состояния АФУ.
typedef void (*callbackStatus)(int data, void* userData);

/// Определение типа функции, которая вызывается при изменении состояния приёма/передачи.
typedef void (*callbackTrx)(bool data, void* userData);

/// Определение типа функции, которая вызывается при плохом контакте.
typedef void (*callbackBadContact)(void* userData);

/// Определение типа функции, которая вызывается при измении текущего значения КСВ.
typedef void (*callbackCurrentSwr)(float watts, float swr, float filteredSwr, void* userData);

#endif // PANORAMAKCOMMON_H
