#include "ServiceLib.h"

Service::Service(QObject* parent)
    : QObject { parent }
{
    m_loader.initialize();
    m_libService = m_loader.createService();

    pControl = std::make_shared<ControlObserver>();

    connect(pControl.get(), &ControlObserver::opened, this, &Service::onOpen);
    connect(pControl.get(), &ControlObserver::closed, this, &Service::onClose);
    connect(pControl.get(), &ControlObserver::protocolChanged, this, &Service::onProtocol);
}

Service::~Service()
{
    m_loader.deleteService(m_libService);
    m_loader.finalize();
}

std::shared_ptr<ControlObserver> Service::control()
{
    return pControl;
}

PowerSwrSeries<PowerSwrData>* Service::series()
{
    return reinterpret_cast<PowerSwrSeries<PowerSwrData>*>(m_loader.series(m_libService));
}

void Service::onProtocol(quint32 index)
{
    m_loader.setProtocol(index, m_libService, this);

    m_loader.setConnectionStatusCallback(connectionStatus, m_libService);
    m_loader.setStatusCallback(status, m_libService);
    m_loader.setFrequencyCallback(frequency, m_libService);
    m_loader.setTrxCallback(trx, m_libService);
    m_loader.setBadContactCallback(badContact, m_libService);
    m_loader.setCurrentSwrCallback(currentSwrValue, m_libService);
}

void Service::onOpen(const QJsonObject& config, bool state)
{
    QJsonDocument document{config};
    std::string string = document.toJson(QJsonDocument::JsonFormat::Compact).toStdString();
    ErrorCode errorCode = m_loader.open(string.c_str(), m_libService);
    if (errorCode != ErrorCode::Success) {
        emit error(m_loader.getErrorText(errorCode));
    }
}

void Service::onClose()
{
    m_loader.close(m_libService);
}

DeviceInfo* Service::getDevices()
{
    return m_loader.getDevices();
}

void Service::connectionStatus(int data, void* userData)
{
    reinterpret_cast<Service*>(userData)->pControl->setConnectionStatus(static_cast<PanoramaK::ConnectionStatus>(data));
}

void Service::status(int data, void* userData)
{
    reinterpret_cast<Service*>(userData)->pControl->setStatus(static_cast<PanoramaK::Status>(data));
}

void Service::frequency(int data, void* userData)
{
    reinterpret_cast<Service*>(userData)->pControl->setFrequency(data);
}

void Service::trx(bool data, void* userData)
{
    reinterpret_cast<Service*>(userData)->pControl->setTrx(data);
}

void Service::badContact(void* userData)
{
    emit reinterpret_cast<Service*>(userData)->pControl->badContactDetectedNotify();
}

void Service::currentSwrValue(float watts, float swr, float filteredSwr, void* userData)
{
    emit reinterpret_cast<Service*>(userData)->currentSwr(swr);
    emit reinterpret_cast<Service*>(userData)->currentWatts(watts);
}
