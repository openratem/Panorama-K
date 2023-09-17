#include "LibLoader.h"
#include <QDebug>

#if defined(_WIN32) || defined(__CYGWIN__)
// windows
#include <Windows.h>
#include <string>

LibLoader::LibLoader()
{
    std::wstring path = QString("%1/PanoramaK.dll").arg(qApp->applicationDirPath()).toStdWString();
    instance = LoadLibraryW(path.data());
    if (!instance) {
        qFatal() << Q_FUNC_INFO << "Failed to load PanoramaK.dll";
    }

    m_initialize = reinterpret_cast<pointerInitialize>(GetProcAddress(instance, "initialize"));
    m_finalize = reinterpret_cast<pointerFinalize>(GetProcAddress(instance, "finalize"));
    m_getErrorText = reinterpret_cast<pointerGetErrorText>(GetProcAddress(instance, "getErrorText"));
    m_createService = reinterpret_cast<pointerCreateService>(GetProcAddress(instance, "createService"));
    m_deleteService = reinterpret_cast<pointerDeleteService>(GetProcAddress(instance, "deleteService"));
    m_series = reinterpret_cast<pointerSeries>(GetProcAddress(instance, "getSeries"));
    m_getDeviceInfo = reinterpret_cast<pointerGetDevices>(GetProcAddress(instance, "getDevices"));
    m_setProtocol = reinterpret_cast<pointerSetProtocol>(GetProcAddress(instance, "setProtocol"));
    m_open = reinterpret_cast<pointerOpen>(GetProcAddress(instance, "deviceOpen"));
    m_close = reinterpret_cast<pointerClose>(GetProcAddress(instance, "deviceClose"));
    m_setFrequencyCallback = reinterpret_cast<pointerSetFrequencyCallback>(GetProcAddress(instance, "setFrequencyCallback"));
    m_setConnectionStatusCallback = reinterpret_cast<pointerSetConnectionStatusCallback>(GetProcAddress(instance, "setConnectionStatusCallback"));
    m_setStatusCallback = reinterpret_cast<pointerSetStatusCallback>(GetProcAddress(instance, "setStatusCallback"));
    m_setTrxCallback = reinterpret_cast<pointerSetTrxCallback>(GetProcAddress(instance, "setTrxCallback"));
    m_setBadContactCallback = reinterpret_cast<pointerSetBadContactCallback>(GetProcAddress(instance, "setBadContactCallback"));
    m_setCurrentSwrCallback = reinterpret_cast<pointerSetCurrentSwrCallback>(GetProcAddress(instance, "setCurrentSwrCallback"));
}

LibLoader::~LibLoader() = default;

#elif defined(__linux__)
#include <dlfcn.h>
// linux
LibLoader::LibLoader()
{
    instance = dlopen("./libPanoramaK.so", RTLD_LAZY);
    if (!instance){
        qFatal() << Q_FUNC_INFO << dlerror();
    }

    m_initialize = reinterpret_cast<pointerInitialize>(dlsym(instance, "initialize"));
    m_finalize = reinterpret_cast<pointerFinalize>(dlsym(instance, "finalize"));
    m_getErrorText = reinterpret_cast<pointerGetErrorText>(dlsym(instance, "getErrorText"));
    m_createService = reinterpret_cast<pointerCreateService>(dlsym(instance, "createService"));
    m_deleteService = reinterpret_cast<pointerDeleteService>(dlsym(instance, "deleteService"));
    m_series = reinterpret_cast<pointerSeries>(dlsym(instance, "getSeries"));
    m_getDeviceInfo = reinterpret_cast<pointerGetDevices>(dlsym(instance, "getDevices"));
    m_setProtocol = reinterpret_cast<pointerSetProtocol>(dlsym(instance, "setProtocol"));
    m_open = reinterpret_cast<pointerOpen>(dlsym(instance, "deviceOpen"));
    m_close = reinterpret_cast<pointerClose>(dlsym(instance, "deviceClose"));
    m_setFrequencyCallback = reinterpret_cast<pointerSetFrequencyCallback>(dlsym(instance, "setFrequencyCallback"));
    m_setConnectionStatusCallback = reinterpret_cast<pointerSetConnectionStatusCallback>(dlsym(instance, "setConnectionStatusCallback"));
    m_setStatusCallback = reinterpret_cast<pointerSetStatusCallback>(dlsym(instance, "setStatusCallback"));
    m_setTrxCallback = reinterpret_cast<pointerSetTrxCallback>(dlsym(instance, "setTrxCallback"));
    m_setBadContactCallback = reinterpret_cast<pointerSetBadContactCallback>(dlsym(instance, "setBadContactCallback"));
    m_setCurrentSwrCallback = reinterpret_cast<pointerSetCurrentSwrCallback>(dlsym(instance, "setCurrentSwrCallback"));
}

LibLoader::~LibLoader()
{
    dlclose(instance);
}
#elif defined(__APPLE__) && defined(__MACH__)
// macOS
#include <dlfcn.h>

LibLoader::LibLoader()
{
    instance = dlopen("../../../libPanoramaK.dylib", RTLD_LAZY);
    if (!instance){
        qFatal() << Q_FUNC_INFO << dlerror();
    }

    m_initialize = reinterpret_cast<pointerInitialize>(dlsym(instance, "initialize"));
    m_finalize = reinterpret_cast<pointerFinalize>(dlsym(instance, "finalize"));
    m_getErrorText = reinterpret_cast<pointerGetErrorText>(dlsym(instance, "getErrorText"));
    m_createService = reinterpret_cast<pointerCreateService>(dlsym(instance, "createService"));
    m_deleteService = reinterpret_cast<pointerDeleteService>(dlsym(instance, "deleteService"));
    m_series = reinterpret_cast<pointerSeries>(dlsym(instance, "returnSeries"));
    m_setProtocol = reinterpret_cast<pointerSetProtocol>(dlsym(instance, "setProtocol"));
    m_open = reinterpret_cast<pointerOpen>(dlsym(instance, "deviceOpen"));
    m_close = reinterpret_cast<pointerClose>(dlsym(instance, "deviceClose"));
    m_setFrequencyCallback = reinterpret_cast<pointerSetFrequencyCallback>(dlsym(instance, "setFrequencyCallback"));
    m_setConnectionStatusCallback = reinterpret_cast<pointerSetConnectionStatusCallback>(dlsym(instance, "setConnectionStatusCallback"));
    m_setStatusCallback = reinterpret_cast<pointerSetStatusCallback>(dlsym(instance, "setStatusCallback"));
    m_setTrxCallback = reinterpret_cast<pointerSetTrxCallback>(dlsym(instance, "setTrxCallback"));
    m_setBadContactCallback = reinterpret_cast<pointerSetBadContactCallback>(dlsym(instance, "setBadContactCallback"));
    m_setCurrentSwrCallback = reinterpret_cast<pointerSetCurrentSwrCallback>(dlsym(instance, "setCurrentSwrCallback"));
}

LibLoader::~LibLoader()
{
    dlclose(instance);
}
#elif defined(unix) || defined(__unix__) || defined(__unix)
// unix
#else
#  error Unknown environment!
#endif


void LibLoader::initialize()
{
    if (m_initialize == nullptr) {
        return;
    }

    m_initialize();
}

void LibLoader::finalize()
{
    if (m_finalize == nullptr) {
        return;
    }

    m_finalize();
}

const char *LibLoader::getErrorText(ErrorCode code)
{
    if (m_getErrorText == nullptr)
        return nullptr;

    return m_getErrorText(code);
}

ServiceDescriptor LibLoader::createService()
{
    if (m_createService == nullptr) {
        return nullptr;
    }

    return m_createService();
}

void LibLoader::deleteService(ServiceDescriptor descriptor)
{
    if (m_getErrorText == nullptr) {
        return;
    }

    m_deleteService(descriptor);
}

void* LibLoader::series(ServiceDescriptor descriptor)
{
    if (m_series == nullptr) {
        return nullptr;
    }

    return m_series(descriptor);
}

DeviceInfo* LibLoader::getDevices()
{
    if (m_getDeviceInfo == nullptr) {
        return nullptr;
    }

    return m_getDeviceInfo();
}

ErrorCode LibLoader::setProtocol(uint32_t index, ServiceDescriptor descriptor, void* userData)
{
    if (m_setProtocol == nullptr) {
        return ErrorCode::NullPointer;
    }

    return m_setProtocol(index, descriptor, userData);
}

ErrorCode LibLoader::open(const char* config, ServiceDescriptor descriptor)
{
    if (m_open == nullptr) {
        return ErrorCode::NullPointer;
    }

    return m_open(config, descriptor);
}

ErrorCode LibLoader::close(ServiceDescriptor descriptor)
{
    if (m_close == nullptr) {
        return ErrorCode::NullPointer;
    }

    return m_close(descriptor);
}

ErrorCode LibLoader::setFrequencyCallback(callbackFrequency newCallback, ServiceDescriptor descriptor)
{
    if (m_setFrequencyCallback == nullptr) {
        return ErrorCode::NullPointer;
    }

    return m_setFrequencyCallback(newCallback, descriptor);
}

ErrorCode LibLoader::setConnectionStatusCallback(callbackConnectionStatus newCallback, ServiceDescriptor descriptor)
{
    if (m_setConnectionStatusCallback == nullptr) {
        return ErrorCode::NullPointer;
    }

    return m_setConnectionStatusCallback(newCallback, descriptor);
}

ErrorCode LibLoader::setStatusCallback(callbackStatus newCallback, ServiceDescriptor descriptor)
{
    if (m_setStatusCallback == nullptr) {
        return ErrorCode::NullPointer;
    }

    return m_setStatusCallback(newCallback, descriptor);
}

ErrorCode LibLoader::setTrxCallback(callbackTrx newCallback, ServiceDescriptor descriptor)
{
    if (m_setTrxCallback == nullptr) {
        return ErrorCode::NullPointer;
    }

    return m_setTrxCallback(newCallback, descriptor);
}

ErrorCode LibLoader::setBadContactCallback(callbackBadContact newCallback, ServiceDescriptor descriptor)
{
    if (m_setBadContactCallback == nullptr) {
        return ErrorCode::NullPointer;
    }

    return m_setBadContactCallback(newCallback, descriptor);
}

ErrorCode LibLoader::setCurrentSwrCallback(callbackCurrentSwr newCallback, ServiceDescriptor descriptor)
{
    if (m_setCurrentSwrCallback == nullptr) {
        return ErrorCode::NullPointer;
    }

    return m_setCurrentSwrCallback(newCallback, descriptor);
}
