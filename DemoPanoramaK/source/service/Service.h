#pragma once

#include <memory>
#include <QObject>
#include <PanoramaK.h>
#include <PowerSwrSeries.h>
#include "../control/ControlObserver.h"

class Service final : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = nullptr);

    std::shared_ptr<ControlObserver> control();

    PowerSwrSeries<PowerSwrData> &series();

private slots:
    void onProtocol(quint32 index);
    void onOpen(const QJsonObject &config);
    void onClose();
    void onPowerSwr(float watts, float swr);

private:
    std::shared_ptr<ControlObserver> pControl;
    std::shared_ptr<PanoramaK::TransceiverAbstract> pTransceiver;
    std::shared_ptr<PanoramaK::PowerSwrAnalizerAbstract> pAnalizer;
    std::unique_ptr<PanoramaK::Manager> pManager;
    PowerSwrSeries<PowerSwrData> m_powerSwrSeries {36000}; // контейнер на 30 минут
};


