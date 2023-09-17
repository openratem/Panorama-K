/**
 * \file Interface.h
 */

#ifndef INTERFACE_H
#define INTERFACE_H

#include "PanoramaKCommon.h"

#if defined(_WIN64) || defined(_WIN32)
#define PANORAMA_API __stdcall
#elif defined(__linux__)
#define PANORAMA_API
#elif defined(__MACH__)
#define PANORAMA_API
#endif

/**
 *  \mainpage PanoramaK library
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Выполняет инициализацию библиотеки.
 * \details Вызывается перед использованием библиотеки.
 * \see panoramaKFinalize()
 */
void PANORAMA_API initialize();

/**
 * \brief Вызывается после завершения работы с библиотекой.
 * \details После вызова этой функции освобождается вся память, которая была выделена.
 * \see panoramaKInitialize()
 */
void PANORAMA_API finalize();

/**
 * \brief Возвращает текстовое описание кода ошибки.
 * \param - код ошибки.
 * \return указатель на текстовую строку.
 */
const char* PANORAMA_API getErrorText(ErrorCode code);

/**
 * \brief Создаёт экземпляр типа, который реализует всю логику.
 * \return дескриптор или nullptr при ошибке.
 */
ServiceDescriptor PANORAMA_API createService();

/**
 * \brief Удаляет экземпляр типа.
 */
void PANORAMA_API deleteService(ServiceDescriptor descriptor);

/**
 * \brief Возвращает указатель на тип, который хранит значения КСВ.
 * \param - указатель на дескриптор устройства, который будет создан.
 * \return указатель на объект или nullptr при ошибке.
 */
void* PANORAMA_API getSeries(ServiceDescriptor descriptor);

/**
 * \brief Возвращает указатель на список доступных устройств.
 * \return указать на односвязный список.
 */
DeviceInfo* PANORAMA_API getDevices();

/**
 * \brief setProtocol - Функция устанавливает новый протокол.
 * \param index - Индекс протокола, который будет установлен.
 * \param descriptor - Дескриптор устройства, в котором будет установлен протокол.
 * \param userData - Данные пользователя, которые будут установлены в дескриптор устройства.
 * \return Success в случае успеха или код ошибки, возникшей при установке протокола.
 */
ErrorCode PANORAMA_API setProtocol(uint32_t index, ServiceDescriptor descriptor, void* userData);

/**
 * \brief deviceOpen - Функция устанавливает соединение с устройством.
 * \param config - Конфиг, который будет установлен.
 * \param descriptor - Дескриптор устройства, с которым будет установлено соединение.
 * \return Success в случае успеха или код ошибки, возникшей при установке соединения.
 */
ErrorCode PANORAMA_API deviceOpen(const char* config, ServiceDescriptor descriptor);

/**
 * \brief deviceClose - Функция закрывает соединение с устройством.
 * \param descriptor - Дескриптор устройства, с которым будет закрыто соединение.
 * \return Success в случае успеха или код ошибки, возникшей при закрытии соединения.
 */
ErrorCode PANORAMA_API deviceClose(ServiceDescriptor descriptor);

/**
 * \brief setFrequencyCallback - Функция устанавливает колбэк-функцию, которая вызывается при изменении частоты.
 * \param newCallback - Указатель на функцию, которая будет установлена.
 * \param descriptor - Дескриптор устройства, в который будет установлена функция.
 * \return Success в случае успеха или код ошибки, возникшей при установке функции.
 */
ErrorCode PANORAMA_API setFrequencyCallback(callbackFrequency newCallback, ServiceDescriptor descriptor);

/**
 * \brief setConnectionStatusCallback - Функция устанавливает колбэк-функцию, которая вызывается при изменении состояния соединения.
 * \param newCallback - Указатель на функцию, которая будет установлена.
 * \param descriptor - Дескриптор устройства, в который будет установлена функция.
 * \return Success в случае успеха или код ошибки, возникшей при установке функции.
 */
ErrorCode PANORAMA_API setConnectionStatusCallback(callbackConnectionStatus newCallback, ServiceDescriptor descriptor);

/**
 * \brief setStatusCallback - Функция устанавливает колбэк-функцию, которая вызывается при изменении состояния АФУ.
 * \param newCallback - Указатель на функцию, которая будет установлена.
 * \param descriptor - Дескриптор устройства, в который будет установлена функция.
 * \return Success в случае успеха или код ошибки, возникшей при установке функции.
 */
ErrorCode PANORAMA_API setStatusCallback(callbackStatus newCallback, ServiceDescriptor descriptor);

/**
 * \brief setTrxCallback - Функция устанавливает колбэк-функцию, которая вызывается при изменении состояния приёма/передачи.
 * \param newCallback - Указатель на функцию, которая будет установлена.
 * \param descriptor - Дескриптор устройства, в который будет установлена функция.
 * \return Success в случае успеха или код ошибки, возникшей при установке функции.
 */
ErrorCode PANORAMA_API setTrxCallback(callbackTrx newCallback, ServiceDescriptor descriptor);

/**
 * \brief setBadContactCallback - Функция устанавливает колбэк-функцию, которая вызывается при плохом контакте.
 * \param newCallback - Указатель на функцию, которая будет установлена.
 * \param descriptor - Дескриптор устройства, в который будет установлена функция.
 * \return Success в случае успеха или код ошибки, возникшей при установке функции.
 */
ErrorCode PANORAMA_API setBadContactCallback(callbackBadContact newCallback, ServiceDescriptor descriptor);

/**
 * \brief setCurrentSwrCallback - Функция устанавливает колбэк-функцию, которая вызывается при плохом контакте.
 * \param newCallback - Указатель на функцию, которая будет установлена.
 * \param descriptor - Десриптор устройства, в который будет установлена функция.
 * \return Success в случае успеха или код ошибки, возникшей при установке функции.
 */
ErrorCode PANORAMA_API setCurrentSwrCallback(callbackCurrentSwr newCallback, ServiceDescriptor descriptor);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // INTERFACE_H
