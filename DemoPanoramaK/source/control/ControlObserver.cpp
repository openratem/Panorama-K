#include "ControlObserver.h"

ControlObserver::ControlObserver(QObject *parent)
  : QObject{parent}
{

}

void ControlObserver::open(const QJsonObject &config)
{
    if (!m_open) {
        m_connectConfig = config;
        m_open = true;
        emit opened(config);
    }
}


void ControlObserver::close()
{
    if (m_open) {
        m_connectConfig = QJsonObject{};
        m_open = false;
        emit closed();
    }
}

bool ControlObserver::isOpen() const noexcept
{
    return m_open;
}

void ControlObserver::setProtocol(quint32 index)
{
    if (m_protocol != index) {
        m_protocol = index;
        emit protocolChanged(index);
    }
}

quint32 ControlObserver::protocol() const noexcept
{
    return m_protocol;
}

void ControlObserver::setConnectionStatus(PanoramaK::ConnectionStatus status)
{
    if (m_connectionStatus != status) {
        m_connectionStatus = status;
        emit connectionStatusChanged(status);
    }
}

PanoramaK::ConnectionStatus ControlObserver::connectionStatus() const noexcept
{
    return m_connectionStatus;
}

void ControlObserver::setStatus(PanoramaK::Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged(m_status);
    }
}

PanoramaK::Status ControlObserver::status() const noexcept
{
    return m_status;
}

void ControlObserver::setFrequency(int Hz)
{
    if (m_frequency != Hz) {
        m_frequency = Hz;
        emit frequencyChanged(Hz);
    }
}

int ControlObserver::frequency() const noexcept
{
    return m_frequency;
}

void ControlObserver::setTrx(bool state)
{
    if (m_trx != state) {
        m_trx = state;
        emit trxChanged(state);
    }
}

bool ControlObserver::trx() const noexcept
{
    return m_trx;
}




