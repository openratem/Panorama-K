#include "LibService.h"

LibService::LibService(QObject* parent)
    : QObject { parent }
{
}

LibService::~LibService() = default;

PowerSwrSeries<PowerSwrData>* LibService::series()
{
    return &m_powerSwrSeries;
}

void LibService::setProtocol(quint32 index, void* userData)
{
    m_userData = userData;

    m_pointerAnalizer.reset();
    m_pointerTransceiver = PanoramaK::TransceiverFactory::create(PanoramaK::TransceiverFactory::devices().at(index).name);

    if (m_pointerTransceiver) {
        if (m_pointerTransceiver->hasAnalizer()) {
            m_pointerAnalizer = m_pointerTransceiver->analizer();
        } else {
            m_pointerAnalizer = PanoramaK::PowerSwrAnalizerFactory::create({});
        }

        connect(m_pointerTransceiver.get(), &PanoramaK::TransceiverAbstract::connectionStatusChanged, this, [&](PanoramaK::ConnectionStatus status) {
            m_connectionStatusCallback(static_cast<int>(status), m_userData);
        });
        connect(m_pointerTransceiver.get(), &PanoramaK::TransceiverAbstract::frequencyChanged, this, [&](int hz) {
            m_frequencyCallback(hz, m_userData);
        });
        connect(m_pointerTransceiver.get(), &PanoramaK::TransceiverAbstract::trxChanged, this, [&](bool trx) {
            m_trxCallback(trx, m_userData);
        });

        if (m_pointerAnalizer) {
            connect(m_pointerAnalizer.get(), &PanoramaK::PowerSwrAnalizerAbstract::powerSwrValue, this, &LibService::m_powerSwr);
        }
    }

    m_pointerManager = std::make_shared<PanoramaK::Manager>(m_pointerAnalizer, m_pointerTransceiver);

    connect(m_pointerManager.get(), &PanoramaK::Manager::statusChanged, this, [&](PanoramaK::Status status) {
        m_statusCallback(static_cast<int>(status), m_userData);
    });
    connect(m_pointerManager.get(), &PanoramaK::Manager::badContactDetected, this, [&]() {
        m_badContactCallback(m_userData);
    });
    connect(m_pointerManager.get(), &PanoramaK::Manager::powerSwrValue, this, [&](float watts, float swr, float filteredSwr) {
        m_currentSwrValue(watts, swr, filteredSwr, m_userData);
    });
}

bool LibService::open(const char* config)
{
    m_powerSwrSeries.reset();

    QString newString { QString::fromStdString({ config }) };
    QJsonObject object { QJsonDocument::fromJson(newString.toUtf8()).object() };

    if (m_pointerTransceiver) {
        if (m_pointerTransceiver->connectTo(object)) {
            return true;
        }
    }

    return false;
}

void LibService::close()
{
    if (m_pointerTransceiver) {
        m_pointerTransceiver->disconnect();
    }
}

void LibService::setFrequencyCallback(callbackFrequency newCallback)
{
    m_frequencyCallback = newCallback;
}

void LibService::setConnectionStatusCallback(callbackConnectionStatus newCallback)
{
    m_connectionStatusCallback = newCallback;
}

void LibService::setStatusCallback(callbackStatus newCallback)
{
    m_statusCallback = newCallback;
}

void LibService::setTrxCallback(callbackTrx newCallback)
{
    m_trxCallback = newCallback;
}

void LibService::setBadContactCallback(callbackBadContact newCallback)
{
    m_badContactCallback = newCallback;
}

void LibService::setCurrentSwrCallback(callbackCurrentSwr newCallback)
{
    m_currentSwrValue = newCallback;
}

void LibService::m_powerSwr(float watts, float swr)
{
    m_powerSwrSeries.push({ watts, swr });
}
