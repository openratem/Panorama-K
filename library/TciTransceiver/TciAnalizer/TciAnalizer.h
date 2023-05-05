#pragma once

#include "../../PanoramaK.h"
#include <TciClient.h>

namespace PanoramaK {

class TciAnalizer : public PowerSwrAnalizerAbstract
{
    Q_OBJECT
public:
    explicit TciAnalizer(TciClient &tci, QObject *parent = nullptr);

    /**
     * \brief Возвращает текущее значение мощности, Вт.
     */
    float power() const override;

    /**
     * \brief Возвращает текущее значение КСВ.
     */
    float swr() const override;

private slots:
    void onTxSensors(quint32 transceiver, double mic, double rms, double peak, double swr);

private:
    TciClient &m_tci;
    float m_power {0.0f};
    float m_swr {1.0f};
    int m_skipCount {0};
};

} // namespace PanoramaK
