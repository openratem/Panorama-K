#pragma once

#include "../PanoramaKCommon.h"
#include <PanoramaK.h>
#include <PowerSwrSeries.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QtCore>
#include <memory>
#include <string>

class LibService : public QObject {
    Q_OBJECT
public:
    explicit LibService(QObject* parent = nullptr);

    ~LibService();

    PowerSwrSeries<PowerSwrData>* series();

    void setProtocol(quint32 index, void* userData);

    bool open(const char* config);

    void close();

    void setFrequencyCallback(callbackFrequency newCallback);

    void setConnectionStatusCallback(callbackConnectionStatus newCallback);

    void setStatusCallback(callbackStatus newCallback);

    void setTrxCallback(callbackTrx newCallback);

    void setBadContactCallback(callbackBadContact newCallback);

    void setCurrentSwrCallback(callbackCurrentSwr newCallback);

private slots:
    void m_powerSwr(float watts, float swr);

private:
    std::shared_ptr<PanoramaK::TransceiverAbstract> m_pointerTransceiver;
    std::shared_ptr<PanoramaK::PowerSwrAnalizerAbstract> m_pointerAnalizer;
    std::shared_ptr<PanoramaK::Manager> m_pointerManager;
    PowerSwrSeries<PowerSwrData> m_powerSwrSeries { 36000 }; // контейнер на 30 минут

    callbackFrequency m_frequencyCallback;
    callbackConnectionStatus m_connectionStatusCallback;
    callbackStatus m_statusCallback;
    callbackTrx m_trxCallback;
    callbackBadContact m_badContactCallback;
    callbackCurrentSwr m_currentSwrValue;

    void* m_userData;
};
