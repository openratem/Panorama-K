#include "Interface.h"

#include "LibService/LibService.h"
#include <OmniRig>
#include <QList>
#include <QVector>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <list>
#include <stdio.h>
#include <string>
#include <thread>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

std::atomic_bool m_init { false };
std::atomic_flag m_lock { ATOMIC_FLAG_INIT };

std::unique_ptr<QCoreApplication> m_pointerCoreApplication;
std::unique_ptr<QEventLoop> m_pointerEventLoop;
std::unique_ptr<std::thread> m_pointerThreadApplication;

std::list<std::unique_ptr<LibService>> m_services;

static std::vector<DeviceInfo> m_devices;

struct DeviceInfoDeleter {
    void operator()(DeviceInfo* p)
    {
        while (p->next != nullptr) {
            delete[] p->name;
            p = p->next;
        }
    }
};

static std::unique_ptr<DeviceInfo, DeviceInfoDeleter> pDeviceList;

void applicationThreadExecute()
{
    if (qApp == nullptr) {
        int argc = 0;
        const char* argv = "";

        m_pointerCoreApplication = std::make_unique<QCoreApplication>(argc, const_cast<char**>(&argv));
        m_lock.clear(std::memory_order_release);
        m_pointerCoreApplication->exec();
        m_pointerCoreApplication.reset();
    } else {
        m_pointerEventLoop = std::make_unique<QEventLoop>();
        m_lock.clear(std::memory_order_release);
        m_pointerEventLoop->exec();
        m_pointerEventLoop.reset();
    }
}

void PANORAMA_API initialize()
{
    if (m_init) {
        return;
    }

    m_lock.test_and_set(std::memory_order_acquire);

    m_pointerThreadApplication = std::make_unique<std::thread>(applicationThreadExecute);
    m_pointerThreadApplication->detach();

    while (m_lock.test_and_set(std::memory_order_acquire)) { }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    m_init = true;
}

void PANORAMA_API finalize()
{
    if (!m_init) {
        return;
    }

    m_services.erase(m_services.begin(), m_services.end());

    if (m_pointerCoreApplication) {
        QMetaObject::invokeMethod(m_pointerCoreApplication.get(), "quit", Qt::QueuedConnection);
    } else if (m_pointerEventLoop) {
        QMetaObject::invokeMethod(m_pointerEventLoop.get(), "quit", Qt::QueuedConnection);
    } else {
        return;
    }

    if (m_pointerThreadApplication->joinable()) {
        m_pointerThreadApplication->join();
    }

    pDeviceList.reset();

    m_init = false;
}

const char* PANORAMA_API getErrorText(ErrorCode code)
{
    static constexpr std::string_view m_textNoError { "No error" };
    static constexpr std::string_view m_textUnknownError { "Unknown error" };
    static constexpr std::string_view m_textDeviceIsNotOpenedError { "Device is not opened" };
    static constexpr std::string_view m_textInvalidDescriptor { "Invalid device descriptor" };
    static constexpr std::string_view m_textNullPointer { "Null pointer" };

    switch (code) {
    case ErrorCode::Success:
        return m_textNoError.data();
    case ErrorCode::DeviceIsNotOpened:
        return m_textDeviceIsNotOpenedError.data();
    case ErrorCode::InvalidDescriptor:
        return m_textInvalidDescriptor.data();
    case ErrorCode::NullPointer:
        return m_textNullPointer.data();
    case ErrorCode::UnknownError:
        return m_textUnknownError.data();
    default:
        return m_textUnknownError.data();
    }
}

ServiceDescriptor PANORAMA_API createService()
{
    m_services.emplace_back(std::make_unique<LibService>());
    return m_services.back().get();
}

void PANORAMA_API deleteService(ServiceDescriptor descriptor)
{
    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end())
        m_services.erase(iterator);
}

void* PANORAMA_API getSeries(ServiceDescriptor descriptor)
{
    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end())
        return (*iterator)->series();
    return nullptr;
}

DeviceInfo* PANORAMA_API getDevices()
{
    if (m_devices.empty()) {
        DeviceInfo t_info;
        t_info.controlType = ControlType::UnknownProtocol;
        t_info.next = nullptr;

        std::string string = "Unknown";
        t_info.name = new char[string.size() + 1];
        std::fill_n(t_info.name, string.size() + 1, 0);
        std::copy(string.begin(), string.end(), t_info.name);
        m_devices.emplace_back(std::move(t_info));

        string = "SunSDR TCI";
        t_info.name = new char[string.size() + 1];
        std::fill_n(t_info.name, string.size() + 1, 0);
        std::copy(string.begin(), string.end(), t_info.name);

        t_info.controlType = ControlType::Network;
        m_devices.push_back(t_info);

        for (QString& title : PanoramaK::OmniRig::instance().protocols()) {
            string = title.toStdString();
            t_info.name = new char[string.size() + 1];
            std::fill_n(t_info.name, string.size() + 1, 0);
            std::copy(string.begin(), string.end(), t_info.name);
            t_info.controlType = ControlType::SerialPort;
            t_info.next = nullptr;
            m_devices.emplace_back(std::move(t_info));
        }

        for (int i { 0 }; i < m_devices.size() - 1; ++i) {
            m_devices.at(i).next = &m_devices.at(i + 1);
        }

        pDeviceList.reset(&m_devices.front());
    }

    return pDeviceList.get();
}

ErrorCode PANORAMA_API setProtocol(uint32_t index, ServiceDescriptor descriptor, void* userData)
{
    if ((userData == nullptr) || (descriptor == nullptr)) {
        return ErrorCode::NullPointer;
    }

    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end()) {
        (*iterator)->setProtocol(index, userData);
        return ErrorCode::Success;
    }

    return ErrorCode::NullPointer;
}

ErrorCode PANORAMA_API deviceOpen(const char* config, ServiceDescriptor descriptor)
{
    if ((config == nullptr) || (descriptor == nullptr)) {
        return ErrorCode::NullPointer;
    }

    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator == m_services.end()) {
        return ErrorCode::InvalidDescriptor;
    }

    if ((*iterator)->open(config)) {
        return ErrorCode::Success;
    }

    return ErrorCode::DeviceIsNotOpened;
}

ErrorCode PANORAMA_API deviceClose(ServiceDescriptor descriptor)
{
    if (descriptor == nullptr) {
        return ErrorCode::NullPointer;
    }

    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end()) {
        (*iterator)->close();
        return ErrorCode::Success;
    }

    return ErrorCode::InvalidDescriptor;
}

ErrorCode PANORAMA_API setFrequencyCallback(callbackFrequency newCallback, ServiceDescriptor descriptor)
{
    if ((newCallback == nullptr) || (descriptor == nullptr)) {
        return ErrorCode::NullPointer;
    }

    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end()) {
        (*iterator)->setFrequencyCallback(newCallback);
        return ErrorCode::Success;
    }

    return ErrorCode::InvalidDescriptor;
}

ErrorCode PANORAMA_API setConnectionStatusCallback(callbackConnectionStatus newCallback, ServiceDescriptor descriptor)
{
    if ((newCallback == nullptr) || (descriptor == nullptr)) {
        return ErrorCode::NullPointer;
    }

    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end()) {
        (*iterator)->setConnectionStatusCallback(newCallback);
        return ErrorCode::Success;
    }

    return ErrorCode::InvalidDescriptor;
}

ErrorCode PANORAMA_API setStatusCallback(callbackStatus newCallback, ServiceDescriptor descriptor)
{
    if ((newCallback == nullptr) || (descriptor == nullptr)) {
        return ErrorCode::NullPointer;
    }

    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end()) {
        (*iterator)->setStatusCallback(newCallback);
        return ErrorCode::Success;
    }

    return ErrorCode::InvalidDescriptor;
}

ErrorCode PANORAMA_API setTrxCallback(callbackTrx newCallback, ServiceDescriptor descriptor)
{
    if ((newCallback == nullptr) || (descriptor == nullptr)) {
        return ErrorCode::NullPointer;
    }

    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end()) {
        (*iterator)->setTrxCallback(newCallback);
        return ErrorCode::Success;
    }

    return ErrorCode::InvalidDescriptor;
}

ErrorCode PANORAMA_API setBadContactCallback(callbackBadContact newCallback, ServiceDescriptor descriptor)
{
    if ((newCallback == nullptr) || (descriptor == nullptr)) {
        return ErrorCode::NullPointer;
    }

    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end()) {
        (*iterator)->setBadContactCallback(newCallback);
        return ErrorCode::Success;
    }

    return ErrorCode::InvalidDescriptor;
}

ErrorCode PANORAMA_API setCurrentSwrCallback(callbackCurrentSwr newCallback, ServiceDescriptor descriptor)
{
    if ((newCallback == nullptr) || (descriptor == nullptr)) {
        return ErrorCode::NullPointer;
    }

    auto iterator = std::find_if(m_services.begin(), m_services.end(), [&](auto& pointer) { return pointer.get() == reinterpret_cast<LibService*>(descriptor); });
    if (iterator != m_services.end()) {
        (*iterator)->setCurrentSwrCallback(newCallback);
        return ErrorCode::Success;
    }

    return ErrorCode::InvalidDescriptor;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
