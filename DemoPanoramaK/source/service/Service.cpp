#include "Service.h"

Service::Service(QObject *parent)
    : QObject{parent}
{
    pControl = std::make_shared<ControlObserver>();

    connect(pControl.get(), &ControlObserver::opened, this, &Service::onOpen);
    connect(pControl.get(), &ControlObserver::closed, this, &Service::onClose);
    connect(pControl.get(), &ControlObserver::protocolChanged, this, &Service::onProtocol);
}

std::shared_ptr<ControlObserver> Service::control()
{
    return pControl;
}

PowerSwrSeries<PowerSwrData>& Service::series()
{
    return m_powerSwrSeries;
}

void Service::onProtocol(quint32 index)
{
    pAnalizer.reset();
    pTransceiver = PanoramaK::TransceiverFactory::create(PanoramaK::TransceiverFactory::devices().at(index).name);

    if (pTransceiver) {
        if (pTransceiver->hasAnalizer())
            pAnalizer = pTransceiver->analizer();
        else
            pAnalizer = PanoramaK::PowerSwrAnalizerFactory::create({});

        connect(pTransceiver.get(), &PanoramaK::TransceiverAbstract::connectionStatusChanged, pControl.get(), &ControlObserver::setConnectionStatus);
        connect(pTransceiver.get(), &PanoramaK::TransceiverAbstract::frequencyChanged, pControl.get(), &ControlObserver::setFrequency);
        connect(pTransceiver.get(), &PanoramaK::TransceiverAbstract::trxChanged, pControl.get(), &ControlObserver::setTrx);

        if (pAnalizer) {
            connect(pAnalizer.get(), &PanoramaK::PowerSwrAnalizerAbstract::powerSwrValue, this, &Service::onPowerSwr);
        }
    }

    pManager = std::make_unique<PanoramaK::Manager>(pAnalizer, pTransceiver);

    connect(pManager.get(), &PanoramaK::Manager::statusChanged, pControl.get(), &ControlObserver::setStatus);
    connect(pManager.get(), &PanoramaK::Manager::badContactDetected, pControl.get(), &ControlObserver::badContactDetectedNotify);
}

void Service::onOpen(const QJsonObject &config)
{
    m_powerSwrSeries.reset();

    if (pTransceiver)
        pTransceiver->connectTo(config);
}

void Service::onClose()
{
    if (pTransceiver)
        pTransceiver->disconnect();
}

void Service::onPowerSwr(float watts, float swr)
{
    m_powerSwrSeries.push({watts, swr});
}






