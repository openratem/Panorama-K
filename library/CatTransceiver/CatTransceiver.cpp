#include "CatTransceiver.h"
#include <QMap>

static const QMap<QString, QSerialPort::Parity> ParityValues {{"Even" , QSerialPort::Parity::EvenParity },
                                                              {"Odd"  , QSerialPort::Parity::OddParity  },
                                                              {"Space", QSerialPort::Parity::SpaceParity},
                                                              {"Mark" , QSerialPort::Parity::MarkParity }};

inline static QSerialPort::Parity toParity(const QString &name) {
    if (ParityValues.contains(name))
        return ParityValues[name];
    return QSerialPort::Parity::NoParity;
}

namespace PanoramaK {

CatTransceiver::CatTransceiver(const QString &protocol, QObject *parent)
  : TransceiverAbstract{parent},
    m_protocol{protocol}
{
    pAnalizer = std::make_shared<CatAnalizer>();
}

CatTransceiver::~CatTransceiver()
{

}

bool PanoramaK::CatTransceiver::hasAnalizer() const
{
    return true;
}

std::shared_ptr<PowerSwrAnalizerAbstract> CatTransceiver::analizer()
{
    return pAnalizer;
}

bool CatTransceiver::connectTo(const QJsonObject &config)
{
    // установка настроек COM порта
    auto comportParams = config["serialPort"].toObject();
    if (comportParams.empty())
        return false;

    m_serialPort.setPortName(comportParams["name"].toString());
    m_serialPort.setParity(toParity(comportParams["parity"].toString()));
    m_serialPort.setDataBits(static_cast<QSerialPort::DataBits>(comportParams["data"].toString().toInt()));
    m_serialPort.setBaudRate(static_cast<QSerialPort::BaudRate>(comportParams["baudrate"].toString().toInt()));
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);

    // открываем порт трансивера
    if (!m_serialPort.open(QSerialPort::ReadWrite))
        return false;

    m_serialPort.setDataTerminalReady(comportParams["dtr"].toInt() != 0);
    m_serialPort.setRequestToSend(comportParams["rts"].toInt() != 0);

    // открываем порт анализатора мощности и КСВ
    if (!pAnalizer->open(config["analizer"].toObject()))
        return false;

    pTransceiver = OmniRig::instance().createRig(m_serialPort, m_protocol);
    connect(pTransceiver.get(), &RigAbstract::connected, this, [&](){ onConnectStatus(ConnectionStatus::Connected); });
    connect(pTransceiver.get(), &RigAbstract::disconnected, this, [&](){ onConnectStatus(ConnectionStatus::Connecting); });
    connect(pTransceiver.get(), &RigAbstract::trxChanged, this, &CatTransceiver::onTrx);
    connect(pTransceiver.get(), &RigAbstract::vfoChanged, this, [&](auto channel, auto Hz){
        if (channel == 0)
            onFrequency(Hz);
    });

    onConnectStatus(ConnectionStatus::Disconnect);

    return true;
}

void CatTransceiver::disconnect()
{
    m_serialPort.close();
    pTransceiver.reset();
    m_trx = false;
    m_frequency = 0;
    onConnectStatus(ConnectionStatus::Disconnect);
}

bool CatTransceiver::isConnected() const
{
    return m_connectStatus == ConnectionStatus::Connected;
}

bool CatTransceiver::trx() const
{
    return m_trx;
}

int CatTransceiver::frequency() const
{
    return m_frequency;
}

void CatTransceiver::turnOffTransmissionMode()
{
    if (pTransceiver)
        pTransceiver->setTrx(false);
}

void CatTransceiver::onConnectStatus(ConnectionStatus status)
{
    if (m_connectStatus != status) {
        m_connectStatus = status;
        emit connectionStatusChanged(status);
    }
}

void CatTransceiver::onTrx(bool state)
{
    if (m_trx != state) {
        m_trx = state;
        emit trxChanged(state);
    }
}

void CatTransceiver::onFrequency(quint32 Hz)
{
    if (m_frequency != Hz) {
        m_frequency = Hz;
        emit frequencyChanged(Hz);
    }
}

} // namespace PanoramaK
