#include "TciTransceiver.h"

namespace PanoramaK {

TciTransceiver::TciTransceiver(QObject* parent)
    : TransceiverAbstract { parent }
{
    pAnalizer = std::make_shared<TciAnalizer>(m_tci);

    connect(&m_tci, &TciClient::statusChanged, this, [&](TciClient::Status status) {
        m_connected = status == TciClient::Status::Connected;

        if (status == TciClient::Status::Connecting)
            emit connectionStatusChanged(ConnectionStatus::Connecting);
        else if (status == TciClient::Status::Connected)
            emit connectionStatusChanged(ConnectionStatus::Connected);
        else
            emit connectionStatusChanged(ConnectionStatus::Disconnect);
    });

    connect(&m_tci, &TciClient::ready, this, [&]() {
        m_tci.setTxSensorsEnable(true, 50); // включаем получение мощности и КСВ с интервалом 50 мс
    });

    connect(&m_tci, &TciClient::ddsChanged, this, [&](auto transceiver, auto Hz) {
        if (transceiver == 0) {
            m_frequency = Hz;
            emit frequencyChanged(Hz);
        }
    });

    connect(&m_tci, &TciClient::trxChanged, this, [&](auto transceiver, auto tx) {
        if (transceiver == 0) {
            m_trx = tx;
            emit trxChanged(tx);
        }
    });

    connect(&m_tci, &TciClient::tuneChanged, this, [&](auto transceiver, auto tx) {
        if (transceiver == 0) {
            m_trx = tx;
            emit trxChanged(tx);
        }
    });
}

bool TciTransceiver::hasAnalizer() const
{
    return true;
}

std::shared_ptr<PowerSwrAnalizerAbstract> TciTransceiver::analizer()
{
    return pAnalizer;
}

bool TciTransceiver::connectTo(const QJsonObject& config)
{
    const auto t_settings = config["network"].toObject();
    if (t_settings.contains("address")) {
        m_tci.connect("ws://" + t_settings["address"].toString());
        return true;
    }
    return false;
}

void TciTransceiver::disconnect()
{
    m_tci.disconnect();
}

bool TciTransceiver::isConnected() const
{
    return false;
}

bool TciTransceiver::trx() const
{
    return m_trx;
}

int TciTransceiver::frequency() const
{
    return m_frequency;
}

void TciTransceiver::turnOffTransmissionMode()
{
    m_tci.setTrx(0, false, QString {});
}

} // namespace PanoramaK
