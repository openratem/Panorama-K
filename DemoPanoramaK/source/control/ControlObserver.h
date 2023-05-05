#pragma once

#include <QObject>
#include <QJsonObject>
#include <PanoramaK.h>

class ControlObserver final : public QObject
{
    Q_OBJECT

public:
    explicit ControlObserver(QObject *parent = nullptr);

    /**
     * \brief Возвращает \a true если происходит подключение к трасиверу или оно уже выполнено, иначе \a false.
     */
    Q_INVOKABLE bool isOpen() const noexcept;

    /**
     * \brief Возвращает выбранный протокол управления трансивером.
     */
    quint32 protocol() const noexcept;

    /**
     * \brief Возвращает статус подключения к трансиверу.
     */
    Q_INVOKABLE PanoramaK::ConnectionStatus connectionStatus() const noexcept;

    /**
     * \brief Возвращает статус состояния АФУ.
     */
    Q_INVOKABLE PanoramaK::Status status() const noexcept;

    /**
     * \brief Возвращает \a true если трансивер в режиме передачи, иначе \a false.
     */
    Q_INVOKABLE bool trx() const noexcept;

    /**
     * \brief Возвращает частоту передатчика, Гц.
     */
    Q_INVOKABLE int frequency() const noexcept;

signals:
    /**
     * \brief Уведомляет о подключении к трансиверу.
     * \param config - параметры подключения.
     */
    void opened(QJsonObject config);

    /**
     * \brief Уведомляет об отключении от трансивера.
     */
    void closed();

    /**
     * \brief Уведомляет об изменении выбранного протокола управления трансивером.
     * \param index - порядковый номер протокола из списка.
     */
    void protocolChanged(quint32 index);

    /**
     * \brief Уведомляет об изменении статуса подключения к трансиверу.
     * \param status - статус подключения.
     */
    void connectionStatusChanged(PanoramaK::ConnectionStatus status);

    /**
     * \brief Уведомляет об изменении статуса состояния АФУ.
     */
    void statusChanged(PanoramaK::Status status);

    /**
     * \brief Уведомляет об обнаружении плохого контакта.
     */
    void badContactDetected();

    /**
     * \brief Уведомляет об изменении режима приём/передача.
     * \param trx - флаг режима передачи, если \a true - режим передачи, иначе \a false.
     */
    void trxChanged(bool trx);

    /**
     * \brief Уведомляет об изменении частоты передатчика, Гц.
     * \param Hz - частота передатчика, Гц.
     */
    void frequencyChanged(int Hz);

    /**
     * \brief Уведомляет об обнаружении плохого контакта.
     */
    void badContactDetectedNotify();

public slots:
    /**
     * \brief Установка подключения к трансиверу.
     * \param config - параметры подключения.
     */
    void open(const QJsonObject &config);

    /**
     * \brief Отключение от трансивера.
     */
    void close();

    /**
     * \brief Установка выбранного протокола управления трансивером.
     * \param index - порядковый номер протокола.
     */
    void setProtocol(quint32 index);

    /**
     * \brief Установка статуса подключения к трансиверу.
     * \param status - статус подключения.
     */
    Q_INVOKABLE void setConnectionStatus(PanoramaK::ConnectionStatus status);

    /**
     * \brief Установка статуса состояния АФУ.
     */
    Q_INVOKABLE void setStatus(PanoramaK::Status status);

    /**
     * \brief Установка режима приём/передача.
     * \param enable - если \a true тогда режим передачи, иначе \a false.
     */
    Q_INVOKABLE void setTrx(bool state);

    /**
     * \brief Установка частоты передатчика, Гц.
     * \param Hz - частота, гц.
     */
    Q_INVOKABLE void setFrequency(int Hz);

private:
    QJsonObject m_connectConfig;
    PanoramaK::ConnectionStatus m_connectionStatus {PanoramaK::ConnectionStatus::Disconnect};
    PanoramaK::Status m_status {PanoramaK::Status::Ok};
    quint32 m_protocol {0};
    int m_frequency {0};
    bool m_trx {false};
    bool m_open {false};
};

