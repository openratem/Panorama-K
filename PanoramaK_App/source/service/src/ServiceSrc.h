#pragma once

#include "../../control/ControlObserver.h"
#include <PowerSwrSeries.h>
#include <QJsonDocument>
#include <QObject>
#include <memory>
#include <string>

class Service final : public QObject {
    Q_OBJECT

public:
    explicit Service(QObject* parent = nullptr);

    ~Service();

    std::shared_ptr<ControlObserver> control();

    PowerSwrSeries<PowerSwrData>* series();

signals:
    void currentSwr(float value);
    void currentWatts(float watts);

private slots:
    void onProtocol(quint32 index);
    void onOpen(const QJsonObject& config);
    void onClose();
    void onPowerSwr(float watts, float swr);

private:
    std::shared_ptr<ControlObserver> pControl;
    std::shared_ptr<PanoramaK::TransceiverAbstract> m_pointerTransceiver;
    std::shared_ptr<PanoramaK::PowerSwrAnalizerAbstract> m_pointerAnalizer;
    std::shared_ptr<PanoramaK::Manager> m_pointerManager;
    PowerSwrSeries<PowerSwrData> m_powerSwrSeries { 36000 }; // контейнер на 30 минут
};
