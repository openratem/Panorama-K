#include "TciAnalizer.h"

namespace PanoramaK {

TciAnalizer::TciAnalizer(TciClient &tci, QObject *parent)
  : PowerSwrAnalizerAbstract{parent}, m_tci{tci}
{
    connect(&m_tci, &TciClient::txSensors, this, &TciAnalizer::onTxSensors);
    connect(&m_tci, &TciClient::trxChanged, this, [&](auto, bool tx){
        if (tx)
            m_skipCount = -10;
    });
}

float TciAnalizer::power() const
{
    return m_power;
}

float TciAnalizer::swr() const
{
    return m_swr;
}

void TciAnalizer::onTxSensors(quint32, double, double, double peak, double swr)
{
    if (++m_skipCount; m_skipCount < 0)
        return;

    m_power = peak;
    m_swr   = swr;

    emit powerSwrValue(m_power, m_swr);
}


} // namespace PanoramaK
