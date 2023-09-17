#include "ControlSubject.h"

ControlSubject::ControlSubject(QObject* parent)
    : QObject { parent }
{
}

void ControlSubject::attach(const std::shared_ptr<ControlObserver>& observer)
{
    m_observers.append(observer);

    observer->setProtocol(m_protocol);
    if (m_open)
        observer->open(m_connectConfig, true);
    else
        observer->close(false);
    observer->setConnectionStatus(m_connectionStatus);
    observer->setStatus(m_status);
    observer->setFrequency(m_frequency);
    observer->setTrx(m_trx);

    connect(observer.get(), &ControlObserver::opened, this, &ControlSubject::open);
    connect(this, &ControlSubject::opened, observer.get(), &ControlObserver::open);
    connect(observer.get(), &ControlObserver::closed, this, &ControlSubject::close);
    connect(this, &ControlSubject::closed, observer.get(), &ControlObserver::close);
    connect(observer.get(), &ControlObserver::protocolChanged, this, &ControlSubject::setProtocol);
    connect(this, &ControlSubject::protocolChanged, observer.get(), &ControlObserver::setProtocol);
    connect(observer.get(), &ControlObserver::connectionStatusChanged, this, &ControlSubject::setConnectionStatus);
    connect(this, &ControlSubject::connectionStatusChanged, observer.get(), &ControlObserver::setConnectionStatus);
    connect(observer.get(), &ControlObserver::statusChanged, this, &ControlSubject::setStatus);
    connect(this, &ControlSubject::statusChanged, observer.get(), &ControlObserver::setStatus);
    connect(observer.get(), &ControlObserver::frequencyChanged, this, &ControlSubject::setFrequency);
    connect(this, &ControlSubject::frequencyChanged, observer.get(), &ControlObserver::setFrequency);
    connect(observer.get(), &ControlObserver::trxChanged, this, &ControlSubject::setTrx);
    connect(this, &ControlSubject::trxChanged, observer.get(), &ControlObserver::setTrx);
    connect(observer.get(), &ControlObserver::badContactDetectedNotify, this, &ControlSubject::badContactDetected);
    connect(this, &ControlSubject::badContactDetected, observer.get(), &ControlObserver::badContactDetected);
}

void ControlSubject::open(const QJsonObject& config, bool state)
{
    if (state != m_open) {
        m_connectConfig = config;
        m_open = state;
        emit opened(config, state);
    }
}

void ControlSubject::close(bool state)
{
    if (state != m_open) {
        m_connectConfig = QJsonObject {};
        m_open = state;
        emit closed(state);
    }
}

bool ControlSubject::isOpen() const noexcept
{
    return m_open;
}

void ControlSubject::setProtocol(quint32 index)
{
    if (m_protocol != index) {
        m_protocol = index;
        emit protocolChanged(index);
    }
}

quint32 ControlSubject::protocol() const noexcept
{
    return m_protocol;
}

void ControlSubject::setConnectionStatus(PanoramaK::ConnectionStatus status)
{
    if (m_connectionStatus != status) {
        m_connectionStatus = status;
        emit connectionStatusChanged(status);
    }
}

PanoramaK::ConnectionStatus ControlSubject::connectionStatus() const noexcept
{
    return m_connectionStatus;
}

void ControlSubject::setStatus(PanoramaK::Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged(m_status);
    }
}

PanoramaK::Status ControlSubject::status() const noexcept
{
    return m_status;
}

void ControlSubject::setFrequency(int Hz)
{
    if (m_frequency != Hz) {
        m_frequency = Hz;
        emit frequencyChanged(Hz);
    }
}

void ControlSubject::setSwr(int value)
{
    if (m_swr != value) {
        m_swr = value;
        emit swrChanged(value);
    }
}

int ControlSubject::frequency() const noexcept
{
    return m_frequency;
}

void ControlSubject::setTrx(bool state)
{
    if (m_trx != state) {
        m_trx = state;
        emit trxChanged(state);
    }
}

bool ControlSubject::trx() const noexcept
{
    return m_trx;
}
