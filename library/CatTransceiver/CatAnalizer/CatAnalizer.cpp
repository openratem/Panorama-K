#include "CatAnalizer.h"
#include <string>
#include <range/v3/all.hpp>
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

CatAnalizer::CatAnalizer(QObject *parent)
  : PowerSwrAnalizerAbstract{parent}
{
    QObject::connect(&m_serialPort, &QSerialPort::readyRead, this, [&](){
        m_text += m_serialPort.readAll().toStdString();
        parse(m_text);
    });
}

bool CatAnalizer::open(const QJsonObject &config)
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

    // Открываем
    if (!m_serialPort.open(QSerialPort::ReadWrite))
        return false;

    return true;
}

void CatAnalizer::close()
{
    m_serialPort.close();
    m_power = 0;
    m_swr = 1;
}

float CatAnalizer::power() const
{
    return m_power;
}

float CatAnalizer::swr() const
{
    return m_swr;
}

void CatAnalizer::parse(std::string_view text)
{
    // находим последний символ конца строки
    const auto index = text.find_last_of('\n');
    if (index == text.npos)
        return;

    // получаем все полные строки
    std::string lines {text.data(), index};

    // из каждой строки извлекаем занчение мощности и КСВ
    auto values = lines | ranges::views::split('\n')
                        | ranges::views::transform([](auto &&range){ return std::string_view(&(*range.begin()), ranges::distance(range)); })
                        | ranges::to<std::vector>();

    if (!values.empty()) {
        auto t_values = QString::fromStdString(std::string{values.back()}).split(",");
        if (t_values.size() == 2)
            onPowerSwr(t_values.first().toFloat(), t_values.last().toFloat());
    }
}

void CatAnalizer::onPowerSwr(float watts, float swr)
{
    if (!qFuzzyCompare(watts, m_power) || !qFuzzyCompare(swr, m_swr)) {
        m_power = qBound<float>(0, watts, 10000);
        m_swr   = qBound<float>(1, swr, 11);
        emit powerSwrValue(m_power, m_swr);
    }
}


} // namespace PanoramaK
