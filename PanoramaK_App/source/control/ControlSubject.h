#pragma once

#include "ControlObserver.h"
#include <QList>
#include <memory>

class ControlSubject final : public QObject
{
    Q_OBJECT

public:
    explicit ControlSubject(QObject* parent = nullptr);

    /**
     * \brief Добавление наблюдателя.
     * \param observer - наблюдатель.
     */
    void attach(const std::shared_ptr<ControlObserver>& observer);

    /**
     * \brief Возвращает \a true если происходит подключение к трасиверу или оно уже выполнено, иначе \a false.
     */
    bool isOpen() const noexcept;

    /**
     * \brief Возвращает выбранный протокол управления трансивером.
     */
    quint32 protocol() const noexcept;

    /**
     * \brief Возвращает статус подключения к трансиверу.
     */
    PanoramaK::ConnectionStatus connectionStatus() const noexcept;

    /**
     * \brief Возвращает статус состояния АФУ.
     */
    PanoramaK::Status status() const noexcept;

    /**
     * \brief Возвращает \a true если трансивер в режиме передачи, иначе \a false.
     */
    bool trx() const noexcept;

    /**
     * \brief Возвращает частоту передатчика, Гц.
     */
    int frequency() const noexcept;

signals:
    /**
     * \brief Уведомляет о подключении к трансиверу.
     * \param config - параметры подключения.
     */
    void opened(QJsonObject config, bool state);

    /**
     * \brief Уведомляет об отключении от трансивера.
     */
    void closed(bool state);

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
     * \brief Уведомляет об изменении КСВ.
     * \param value - значение КСВ.
     */
    void swrChanged(int value);

public slots:
    /**
     * \brief Установка подключения к трансиверу.
     * \param config - параметры подключения.
     */
    void open(const QJsonObject& config, bool state);

    /**
     * \brief Отключение от трансивера.
     */
    void close(bool state);

    /**
     * \brief Установка выбранного протокола управления трансивером.
     * \param index - порядковый номер протокола.
     */
    void setProtocol(quint32 index);

    /**
     * \brief Установка статуса подключения к трансиверу.
     * \param status - статус подключения.
     */
    void setConnectionStatus(PanoramaK::ConnectionStatus status);

    /**
     * \brief Установка статуса состояния АФУ.
     */
    void setStatus(PanoramaK::Status status);

    /**
     * \brief Установка режима приём/передача.
     * \param enable - если \a true тогда режим передачи, иначе \a false.
     */
    void setTrx(bool state);

    /**
     * \brief Установка частоты передатчика, Гц.
     * \param Hz - частота, гц.
     */
    void setFrequency(int Hz);

    /**
     * \brief Установка КСВ.
     * \param value - значение КСВ.
     */
    void setSwr(int value);

private:
    QJsonObject m_connectConfig;
    PanoramaK::ConnectionStatus m_connectionStatus { PanoramaK::ConnectionStatus::Disconnect };
    PanoramaK::Status m_status { PanoramaK::Status::Ok };
    quint32 m_protocol { 0 };
    int m_frequency { 0 };
    float m_swr { 1.0f };
    bool m_trx { false };
    bool m_open { false };

    QList<std::shared_ptr<ControlObserver>> m_observers;
};
