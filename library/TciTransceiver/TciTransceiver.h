#pragma once

#include "../PanoramaK.h"
#include <TciClient.h>
#include "TciAnalizer/TciAnalizer.h"

namespace PanoramaK {

class TciTransceiver final : public TransceiverAbstract
{
    Q_OBJECT

public:
    explicit TciTransceiver(QObject *parent = nullptr);

    /**
     * \brief Возвращает \a true если есть встроенный измеритель мощности и КСВ, иначе \a false.
     */
    bool hasAnalizer() const override;

    /**
     * \brief Возвращает экземпляр класса измерителя мощности и КСВ.
     */
    std::shared_ptr<PowerSwrAnalizerAbstract> analizer() override;

    /**
     * \brief Подключение к устройству.
     * \param config - параметры подключения.
     * \return \a true если настройки подключения корректны, иначе \a false.
     */
    bool connectTo(const QJsonObject &config) override;

    /**
     * \brief Отключение от устройства.
     */
    void disconnect() override;

    /**
     * \brief Возвращает \a true если установлено подключение к трансиверу, иначе \a false.
     */
    bool isConnected() const override;

    /**
     * \brief Возвращает \a true если трансивер в режиме передачи, иначе \a false.
     */
    bool trx() const override;

    /**
     * \brief Возвращает частоту передатчика, Гц.
     */
    int frequency() const override;

    /**
     * \brief Выключение режима передачи.
     */
    void turnOffTransmissionMode() override;

private:
    TciClient m_tci;
    int m_frequency {0};
    bool m_trx {false};
    bool m_connected {false};

    std::shared_ptr<TciAnalizer> pAnalizer;
};

} // namespace PanoramaK
