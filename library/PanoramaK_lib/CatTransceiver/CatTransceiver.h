#pragma once

#include "../PanoramaK.h"
#include <OmniRig>
#include <QSerialPort>
#include "CatAnalizer/CatAnalizer.h"

namespace PanoramaK {

class CatTransceiver final : public TransceiverAbstract
{
    Q_OBJECT
public:
    explicit CatTransceiver(const QString &protocol, QObject *parent = nullptr);

    ~CatTransceiver() override;

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

private slots:
    void onConnectStatus(ConnectionStatus status);
    void onTrx(bool state);
    void onFrequency(quint32 Hz);

private:
    QSerialPort m_serialPort;
    std::unique_ptr<RigAbstract> pTransceiver;
    std::shared_ptr<CatAnalizer> pAnalizer;
    QString m_protocol;
    ConnectionStatus m_connectStatus {ConnectionStatus::Disconnect};
    quint32 m_frequency {0};
    bool m_trx {false};
};

} // namespace PanoramaK
