#ifndef LIBLOADER_H
#define LIBLOADER_H

#include "PanoramaKCommon.h"
#include <string>

class LibLoader {
    typedef void (*pointerInitialize)();
    typedef void (*pointerFinalize)();
    typedef const char* (*pointerGetErrorText)(ErrorCode code);
    typedef ServiceDescriptor (*pointerCreateService)();
    typedef void (*pointerDeleteService)(ServiceDescriptor descriptor);
    typedef void* (*pointerSeries)(ServiceDescriptor descriptor);
    typedef DeviceInfo* (*pointerGetDevices)();
    typedef ErrorCode (*pointerSetProtocol)(uint32_t index, ServiceDescriptor descriptor, void* userData);
    typedef ErrorCode (*pointerOpen)(const char* config, ServiceDescriptor descriptor);
    typedef ErrorCode (*pointerClose)(ServiceDescriptor descriptor);
    typedef ErrorCode (*pointerSetFrequencyCallback)(callbackFrequency newCallback, ServiceDescriptor descriptor);
    typedef ErrorCode (*pointerSetConnectionStatusCallback)(callbackConnectionStatus newCallback, ServiceDescriptor descriptor);
    typedef ErrorCode (*pointerSetStatusCallback)(callbackStatus newCallback, ServiceDescriptor descriptor);
    typedef ErrorCode (*pointerSetTrxCallback)(callbackTrx newCallback, ServiceDescriptor descriptor);
    typedef ErrorCode (*pointerSetBadContactCallback)(callbackBadContact newCallback, ServiceDescriptor descriptor);
    typedef ErrorCode (*pointerSetCurrentSwrCallback)(callbackCurrentSwr newCallback, ServiceDescriptor descriptor);

public:
    explicit LibLoader(std::string path);

    ~LibLoader();

    void initialize();

    void finalize();

    const char* getErrorText(ErrorCode code);

    ServiceDescriptor createService();

    void deleteService(ServiceDescriptor descriptor);

    void* series(ServiceDescriptor descriptor);

    DeviceInfo* getDevices();

    ErrorCode setProtocol(uint32_t index, ServiceDescriptor descriptor, void* userData);

    ErrorCode open(const char* config, ServiceDescriptor descriptor);

    ErrorCode close(ServiceDescriptor descriptor);

    ErrorCode setFrequencyCallback(callbackFrequency newCallback, ServiceDescriptor descriptor);

    ErrorCode setConnectionStatusCallback(callbackConnectionStatus newCallback, ServiceDescriptor descriptor);

    ErrorCode setStatusCallback(callbackStatus newCallback, ServiceDescriptor descriptor);

    ErrorCode setTrxCallback(callbackTrx newCallback, ServiceDescriptor descriptor);

    ErrorCode setBadContactCallback(callbackBadContact newCallback, ServiceDescriptor descriptor);

    ErrorCode setCurrentSwrCallback(callbackCurrentSwr newCallback, ServiceDescriptor descriptor);

private:
    void* instance { nullptr };
    pointerInitialize m_initialize { nullptr };
    pointerFinalize m_finalize { nullptr };
    pointerGetErrorText m_getErrorText { nullptr };
    pointerCreateService m_createService { nullptr };
    pointerDeleteService m_deleteService { nullptr };
    pointerSeries m_series { nullptr };
    pointerGetDevices m_getDeviceInfo { nullptr };
    pointerSetProtocol m_setProtocol { nullptr };
    pointerOpen m_open { nullptr };
    pointerClose m_close { nullptr };
    pointerSetFrequencyCallback m_setFrequencyCallback { nullptr };
    pointerSetConnectionStatusCallback m_setConnectionStatusCallback { nullptr };
    pointerSetStatusCallback m_setStatusCallback { nullptr };
    pointerSetTrxCallback m_setTrxCallback { nullptr };
    pointerSetBadContactCallback m_setBadContactCallback { nullptr };
    pointerSetCurrentSwrCallback m_setCurrentSwrCallback { nullptr };
};

#endif // LIBLOADER_H
