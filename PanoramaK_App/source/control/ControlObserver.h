#pragma once

#include <QJsonObject>
#include <QObject>
#ifndef PANORAMAK_LIB_LOADER
#  include <PanoramaK.h>
#else

namespace PanoramaK {
Q_NAMESPACE;

/**
 * \enum Status
 * \brief Перечисление состояний АФУ.
 * \details При анализе показаний датчиков Manager определяет состояние АФУ
 * и уведомляет основную программу через именованные константы.
 */
enum class Status : int {
    Unknown = 0,        ///< Состояние АФУ не определено
    Ok = 1,             ///< Всё хорошо (КСВ 1.0 - 1.3)
    ValidSwr = 2,       ///< Допустимое значение КСВ (КСВ 1.3 - 1.5)
    HighSwr = 4,        ///< Высокое значение КСВ (КСВ 1.6 - 2.0, требуется сервисное обслуживание)
    VeryHighSwr = 8,    ///< Очень высокое значение КСВ (КСВ 2.1 - 5.0, требуется сервисное обслуживание, дальнейшее использование может привести к выходу из строя аппаратуры)
    CableBreakage = 16, ///< Обрыв кабеля (требуется сервисное обслуживание, работа невозможна)
    BadContact = 32     ///< Плохой контакт (искрение, требуется сервисное обслуживание, дальнейшее использование может привести к выходу из строя аппаратуры)
};
Q_ENUM_NS(Status)

/**
 * \enum ConnectionStatus
 * \brief Перечисление состояний подключения к трансиверу.
 */
enum class ConnectionStatus : int {
    Disconnect = 0, ///< Подключение к трансиверу отсутствует
    Connecting,     ///< Выполняется подключение к трансиверу
    Connected   ///< Подключение к трансиверу выполнено
};
Q_ENUM_NS(ConnectionStatus)

} // namespace PanoramaK

#endif

class ControlObserver final : public QObject
{
    Q_OBJECT

public:
    explicit ControlObserver(QObject* parent = nullptr);

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

    /**
     * \brief Возвращает КСВ.
     */
    Q_INVOKABLE float swr() const noexcept;

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

    /**
     * \brief Уведомляет об обнаружении плохого контакта.
     */
    void badContactDetectedNotify();

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

    /**
     * \brief Установка КСВ.
     * \param value - значение КСВ.
     */
    Q_INVOKABLE void setSwr(int value);

private:
    QJsonObject m_connectConfig;
    PanoramaK::ConnectionStatus m_connectionStatus { PanoramaK::ConnectionStatus::Disconnect };
    PanoramaK::Status m_status { PanoramaK::Status::Ok };
    quint32 m_protocol { 0 };
    int m_frequency { 0 };
    float m_swr { 1.0f };
    bool m_trx { false };
    bool m_open { false };
};
