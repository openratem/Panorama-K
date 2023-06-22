#pragma once

#include <memory>
#include <QObject>
#include <QJsonObject>
#include <QStringView>
#include <QStringList>
#include <QDataStream>

namespace PanoramaK
{
Q_NAMESPACE;

/**
 * \enum Status
 * \brief Перечисление состояний АФУ.
 * \details При анализе показаний датчиков Manager определяет состояние АФУ
 * и уведомляет основную программу через именованные константы.
 */
enum class Status : int {
    Unknown         = 0,   ///< Состояние АФУ не определено
    Ok              = 1,   ///< Всё хорошо (КСВ 1.0 - 1.3)
    ValidSwr        = 2,   ///< Допустимое значение КСВ (КСВ 1.3 - 1.5)
    HighSwr         = 4,   ///< Высокое значение КСВ (КСВ 1.6 - 2.0, требуется сервисное обслуживание)
    VeryHighSwr     = 8,   ///< Очень высокое значение КСВ (КСВ 2.1 - 5.0, требуется сервисное обслуживание, дальнейшее использование может привести к выходу из строя аппаратуры)
    CableBreakage   = 16,  ///< Обрыв кабеля (требуется сервисное обслуживание, работа невозможна)
    BadContact      = 32   ///< Плохой контакт (искрение, требуется сервисное обслуживание, дальнейшее использование может привести к выходу из строя аппаратуры)
};
Q_ENUM_NS(Status)


/**
 * \enum ConnectionStatus
 * \brief Перечисление состояний подключения к трансиверу.
 */
enum class ConnectionStatus : int {
    Disconnect = 0,    ///< Подключение к трансиверу отсутствует
    Connecting,        ///< Выполняется подключение к трансиверу
    Connected          ///< Подключение к трансиверу выполнено
};
Q_ENUM_NS(ConnectionStatus)

/**
 * \enum ControlInterface
 * \brief Перечисление интерфейсов управления трансивером.
 */
enum class ControlInterface : int {
    Unknown     = 0,  ///< Неизвестный протокол
    Network     = 1,  ///< Управление по сети
    SerialPort  = 2   ///< Управление через COM порт
};
Q_ENUM_NS(ControlInterface)

/**
 * \struct Protocol
 * \brief Структура описывающая протокол управления трансивером.
 */
struct Protocol {
    QString name;               ///< Название трансивера
    ControlInterface interface; ///< Интерфейс управления
};

/**
 * \class PowerSwrAnalizerAbstract
 * \brief Интерфейс над физическим измерителем мощности и КСВ.
 */
class PowerSwrAnalizerAbstract : public QObject
{
    Q_OBJECT

public:
    /**
     * \brief Конструктор по умолчанию.
     * \param parent - указатель на родительский класс.
     */
    PowerSwrAnalizerAbstract(QObject *parent = nullptr) : QObject{parent} {}

    /**
     * \brief Возвращает текущее значение мощности, Вт.
     */
    virtual float power() const = 0;

    /**
     * \brief Возвращает текущее значение КСВ.
     */
    virtual float swr() const = 0;

signals:
    /**
     * \brief Высылает значения датчиков мощности и КСВ.
     * \param watts - значение мощности, Вт.
     * \param swr - значение КСВ.
     */
    void powerSwrValue(float watts, float swr);
};


/**
 * \class TransceiverAbstract
 * \brief Интерфейс трансивера.
 */
class TransceiverAbstract : public QObject
{
    Q_OBJECT

public:
    /**
     * \brief Конструктор по умолчанию.
     * \param parent - указатель на родительский класс.
     */
    TransceiverAbstract(QObject *parent = nullptr) : QObject{parent} {}

    /**
     * \brief Возвращает \a true если есть встроенный измеритель мощности и КСВ, иначе \a false.
     */
    virtual bool hasAnalizer() const = 0;

    /**
     * \brief Возвращает экземпляр класса измерителя мощности и КСВ.
     */
    virtual std::shared_ptr<PowerSwrAnalizerAbstract> analizer() = 0;

    /**
     * \brief Подключение к устройству.
     * \param config - параметры подключения.
     * \return \a true если настройки подключения корректны, иначе \a false.
     */
    virtual bool connectTo(const QJsonObject &config) = 0;

    /**
     * \brief Отключение от устройства.
     */
    virtual void disconnect() = 0;

    /**
     * \brief Возвращает \a true если установлено подключение к трансиверу, иначе \a false.
     */
    virtual bool isConnected() const = 0;

    /**
     * \brief Возвращает \a true если трансивер в режиме передачи, иначе \a false.
     */
    virtual bool trx() const = 0;

    /**
     * \brief Возвращает частоту передатчика, Гц.
     */
    virtual int frequency() const = 0;

    /**
     * \brief Выключение режима передачи.
     */
    virtual void turnOffTransmissionMode() = 0;

signals:
    /**
     * \brief Уведомляет, когда установлено подключение к трансиверу.
     */
    void connected();

    /**
     * \brief Уведомляет, когда подключение к трансиверу разорвано.
     */
    void disconnected();

    /**
     * \brief Уведомляет об изменении статуса подключения к трансиверу.
     * \param status - статус подключения.
     */
    void connectionStatusChanged(ConnectionStatus status);

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
};


/**
 * \class TransceiverFactory
 * \brief Создаёт экземпляр класс, для управления трансивером.
 */
class TransceiverFactory final
{
public:
    TransceiverFactory();

    /**
     * \brief Возвращает список поддерживаемых моделей трансиверов.
     */
    static QList<Protocol> devices();

    /**
     * \brief Возвращает созданный экземпляр класса управления трансивером.
     * \param device - название модели трансивера.
     * \details Если указана модель, которая отсутствует в списке поддерживаемых, возвращает пустой указатель.
     */
    static std::shared_ptr<TransceiverAbstract> create(const QString &device);
};


/**
 * \class PowerSwrAnalizerFactory
 * \brief Создаёт экземпляр класса, который получает значения мощности и КСВ из внешнего устройства.
 */
class PowerSwrAnalizerFactory final
{
public:
    PowerSwrAnalizerFactory() = delete;

    /**
     * \brief Возвращает экземпляр класса, работающий с внешним измерителем мощности и КСВ.
     */
    static std::shared_ptr<PowerSwrAnalizerAbstract> create(const QJsonObject &param);
};


/**
 * \class Manager
 * \brief Класс выполняет наблюдение за работой устройства, уведомляет о состоянии АФУ и выполняет
 * принудительное выключение режима передачи при обнаружении неисправности АФУ.
 */
class Manager final : public QObject
{
    Q_OBJECT

public:
    Manager() = delete;
    Manager(const Manager &) = delete;
    Manager& operator=(const Manager &) = delete;

    /**
     * \brief Конструктор класса по умолчанию.
     * \param analizer - указатель на измериткль мощности и КСВ.
     * \param transceiver - указатель на интерфейс трансивера.
     * \param parent - указатель на родительский класс.
     */
    Manager(const std::shared_ptr<PowerSwrAnalizerAbstract> &analizer,
            const std::shared_ptr<TransceiverAbstract> &transceiver,
            QObject *parent = nullptr);

    /**
     * \brief Деструктор класса.
     */
    ~Manager() override;

signals:
    /**
     * \brief Уведомляет об изменении состояния АФУ.
     */
    void statusChanged(PanoramaK::Status status);

    /**
     * \brief Высылает значения датчиков мощности и КСВ.
     * \param watts - значение мощности, Вт.
     * \param swr - значение КСВ.
     */
    void powerSwrValue(float watts, float swr, float filteredSwr);

    /**
     * \brief Уведомляет об обнаружении плохого контакта.
     */
    void badContactDetected();

private:
    struct impl;
    std::unique_ptr<impl> pImpl;
};


} // namespace panorama_k


inline QDataStream& operator<<(QDataStream &out, const PanoramaK::Status &status)
{
    out << static_cast<int>(status);
    return out;
}

inline QDataStream& operator>>(QDataStream &in, PanoramaK::Status &status)
{
    int val {0};
    in >> val;
    status = static_cast<PanoramaK::Status>(val);
    return in;
}


inline QDataStream& operator<<(QDataStream &out, const PanoramaK::ConnectionStatus &status)
{
    out << static_cast<int>(status);
    return out;
}

inline QDataStream& operator>>(QDataStream &in, PanoramaK::ConnectionStatus &status)
{
    int val {0};
    in >> val;
    status = static_cast<PanoramaK::ConnectionStatus>(val);
    return in;
}
