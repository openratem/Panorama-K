#include "ServiceSrc.h"

Service::Service(QObject* parent)
    : QObject { parent }
{
    pControl = std::make_shared<ControlObserver>();

    connect(pControl.get(), &ControlObserver::opened, this, &Service::onOpen);
    connect(pControl.get(), &ControlObserver::closed, this, &Service::onClose);
    connect(pControl.get(), &ControlObserver::protocolChanged, this, &Service::onProtocol);
}

Service::~Service() = default;

std::shared_ptr<ControlObserver> Service::control()
{
    return pControl;
}

PowerSwrSeries<PowerSwrData>* Service::series()
{
    return &m_powerSwrSeries;
}

void Service::onProtocol(quint32 index)
{
    m_pointerAnalizer.reset();
    m_pointerTransceiver = PanoramaK::TransceiverFactory::create(PanoramaK::TransceiverFactory::devices().at(index).name);

    if (m_pointerTransceiver) {
        if (m_pointerTransceiver->hasAnalizer()) {
            m_pointerAnalizer = m_pointerTransceiver->analizer();
        } else {
            m_pointerAnalizer = PanoramaK::PowerSwrAnalizerFactory::create({});
        }

        connect(m_pointerTransceiver.get(), &PanoramaK::TransceiverAbstract::connectionStatusChanged, pControl.get(), &ControlObserver::setConnectionStatus);
        connect(m_pointerTransceiver.get(), &PanoramaK::TransceiverAbstract::frequencyChanged, pControl.get(), &ControlObserver::setFrequency);
        connect(m_pointerTransceiver.get(), &PanoramaK::TransceiverAbstract::trxChanged, pControl.get(), &ControlObserver::setTrx);

        if (m_pointerAnalizer) {
            connect(m_pointerAnalizer.get(), &PanoramaK::PowerSwrAnalizerAbstract::powerSwrValue, this, &Service::onPowerSwr);
        }
    }

    m_pointerManager = std::make_shared<PanoramaK::Manager>(m_pointerAnalizer, m_pointerTransceiver);

    connect(m_pointerManager.get(), &PanoramaK::Manager::statusChanged, pControl.get(), &ControlObserver::setStatus);
    connect(m_pointerManager.get(), &PanoramaK::Manager::badContactDetected, pControl.get(), &ControlObserver::badContactDetectedNotify);
    connect(m_pointerManager.get(), &PanoramaK::Manager::powerSwrValue, this, [&](float watts, float swr, float filteredSwr) {
        emit currentSwr(swr);
        emit currentWatts(watts);
    });
}

void Service::onOpen(const QJsonObject& config)
{
    m_powerSwrSeries.reset();

    if (m_pointerTransceiver) {
        m_pointerTransceiver->connectTo(config);
    }
}

void Service::onClose()
{
    if (m_pointerTransceiver) {
        m_pointerTransceiver->disconnect();
    }
}

void Service::onPowerSwr(float watts, float swr)
{
    m_powerSwrSeries.push({ watts, swr });
}
