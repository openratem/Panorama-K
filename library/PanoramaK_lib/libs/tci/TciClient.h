#pragma once

#include <functional>
#include <complex>
#include <memory>
#include <QObject>
#include <span>

/**
 * \class TciClient
 * \brief Класс TCI клиента, выполняет подключение к TCI серверу.
 */
class TciClient final : public QObject
{
    Q_OBJECT

public:
    enum class Status : quint32 {
        Disconnected,
        Connecting,
        Connected
    };

    enum class AgcMode : quint32 {
        Normal,
        Fast,
        Off
    };

public:
    /**
     * \brief Конструктор класса по умолчанию.
     * \param parent - родительский класс.
     */
    explicit TciClient(QObject *parent = nullptr);

    /**
     * \brief Деструктор класса.
     */
    ~TciClient() override;

    /**
     * \brief Возвращает статус подключения
     */
    [[nodiscard]] TciClient::Status status() const noexcept;

signals:
    /**
     * \brief Уведомляет об изменении статуса подключения.
     * \param status - статус подключения.
     */
    void statusChanged(TciClient::Status status);

    /**
     * \brief Уведомляет об изменении протокола TCI.
     * \param name - название ПО.
     * \param version - версия протокола.
     * \details Высылается при подключении.
     */
    void protocolChanged(QString name, QString version);

    /**
     * \brief Уведомляет об изменении названия устройства.
     * \param name - название устройства.
     * \details Высылается при подключении.
     */
    void deviceNameChanged(QString name);

    /**
     * \brief Уведомляет о типе устройства (трансивер или приёмник).
     * \param enable - \a true если приёмник, иначе трансивер.
     * \details Высылается при подключении.
     */
    void receiveOnlyChanged(bool enable);

    /**
     * \brief Уведомляет о количестве трансиверов (приёмников) в устройстве.
     * \param count - количество трансиверов (приёмников).
     * \details Высылается при подключении.
     */
    void trxCountChanged(quint32 count);

    /**
     * \brief Уведомляет о количестве каналов приёма в одном приёмнике.
     * \param count - количество каналов приёма.
     * \details Высылается при подключении.
     */
    void channelsCountChanged(quint32 count);

    /**
     * \brief Уведомляет об изменении диапазона рабочих частот.
     * \param low - нижняя частота, Гц.
     * \param high - верхняя частота, Гц.
     * \details Высылается один раз при подключении.
     */
    void vfoLimitsChanged(double low, double high);

    /**
     * \brief Уведомляет об изменении диапазона рабочих частот фильтра ПЧ.
     * \param low - нижняя частота, Гц.
     * \param high - верхняя частота, Гц.
     * \details Высылается при подключении и изменении частоты дискретизации.
     */
    void ifLimitsChanged(double low, double high);

    /**
     * \brief Уведомляет об изменении списка поддерживаемых модуляций.
     * \param list - список модуляций (названий).
     * \details Высылается при подключении.
     */
    void modulationsListChanged(QStringList list);

    /**
     * \brief Уведомляет, что сервер готов к приёму команд от клиента.
     * \details Высылается после команд инициализации.
     */
    void ready();

    /**
     * \brief Уведомляет, что выполнен старт устройства.
     *
     * \see start()
     * \see stop()
     * \see stoped()
     */
    void started();

    /**
     * \brief Уведомляет, что выполнена остановка устройства.
     *
     * \see start()
     * \see stop()
     * \see started()
     */
    void stoped();

    /**
     * \brief Уведомляет об изменении центральной частоты, Гц.
     * \param transceiver - номер трансивера.
     * \param Hz - центральная частота, Гц.
     */
    void ddsChanged(quint32 transceiver, double Hz);

    /**
     * \brief Уведомляет об изменении частоты настройки ПЧ канала трансивера, Гц.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param Hz - частота, Гц.
     */
    void ifChanged(quint32 transceiver, quint32 channel, double Hz);

    /**
     * \brief Уведомляет об изменении частоты настройки канала трансивера, Гц.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param Hz - частота, Гц.
     */
    void vfoChanged(quint32 transceiver, quint32 channel, double Hz);

    /**
     * \brief Уведомляет об изменении вида связи.
     * \param transceiver - номер трансивера.
     * \param modulation - модуляция.
     */
    void modulationChanged(quint32 transceiver, QString modulation);

    /**
     * \brief Уведомляет об изменении режима работы трансиверу.
     * \param transceiver - номер трансивера.
     * \param enable - если \a true - передача, иначе режим приёма.
     */
    void trxChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении режима работы трансиверу.
     * \param transceiver - номер трансивера.
     * \param enable - если \a true - излучение несущего колебания, иначе режим приёма.
     */
    void tuneChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении мощности передатчика.
     * \param transceiver - номер трансивера.
     * \param value - значение мощности передатчика.
     * \details Значение мощности принимает значения от 0 до 100.
     */
    void driveChanged(quint32 transceiver, double value);

    /**
     * \brief Уведомляет об изменении мощности передатчика в режиме излучения несущей.
     * \param transceiver - номер трансивера.
     * \param value - значение мощности передатчика.
     * \details Значение мощности принимает значения от 0 до 100.
     */
    void tuneDriveChanged(quint32 transceiver, double value);

    /**
     * \brief Уведомляет об изменении флага включения расстройки частоты приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void ritEnabledChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения расстройки частоты передатчика.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void xitEnabledChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения SPLIT.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void splitEnabledChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении частоты расстройки приёмника.
     * \param transceiver - номер трансивера.
     * \param Hz - частота расстройки.
     */
    void ritOffsetChanged(quint32 transceiver, double Hz);

    /**
     * \brief Уведомляет об изменении частоты расстройки передатчика.
     * \param transceiver - номер трансивера.
     * \param Hz - частота расстройки.
     */
    void xitOffsetChanged(quint32 transceiver, double Hz);

    /**
     * \brief Уведомляет об изменении флага включения программного трансивера.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     * \details Первый трансивер всегда включён.
     */
    void transceiverEnabledChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения SPLIT.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param enable - флаг включения.
     * \details Первый канала всегда включён.
     */
    void rxChannelEnabledChanged(quint32 transceiver, quint32 channel, bool enable);

    /**
     * \brief Уведомляет об изменении полосы приёмного фильтра.
     * \param transceiver - номер трансивера.
     * \param low - номер канала.
     * \param high - флаг включения.
     */
    void rxFilterBandChanged(quint32 transceiver, double low, double high);

    /**
     * \brief Уведомляет об изменении скорости телеграфирования макроса.
     * \param wpm - скорость телеграфирования.
     */
    void cwMacrosSpeedChanged(quint32 wpm);

    /**
     * \brief Уведомляет об изменении задержки перед началом передачи макроса.
     * \param ms - задержка перед началом передачи макроса.
     */
    void cwMacrosDelayChanged(quint32 ms);

    /**
     * \brief Уведомляет об изменении скорости телеграфирования ключом.
     * \param wpm - скорость телеграфирования.
     */
    void cwKeyerSpeedChanged(quint32 wpm);

    /**
     * \brief Уведомляет об изменении общей громкости.
     * \param dB - громкость, дБ.
     */
    void volumeChanged(double dB);

    /**
     * \brief Уведомляет об изменении громкости канала приёмника.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param dB - громкость, дБ.
     */
    void rxVolumeChanged(quint32 transceiver, quint32 channel, double dB);

    /**
     * \brief Уведомляет об изменении баланса громкости канала приёмника.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param dB - громкость, дБ.
     */
    void rxBalanceChanged(quint32 transceiver, quint32 channel, double dB);

    /**
     * \brief Уведомляет об изменении флага выключения общей громкости.
     * \param enable - флаг выключения.
     */
    void muteChanged(bool enable);

    /**
     * \brief Уведомляет об изменении флага выключения громкости приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг выключения.
     */
    void rxMuteChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения самоконтроля.
     * \param enable - флаг включения.
     */
    void monEnableChanged(bool enable);

    /**
     * \brief Уведомляет об изменении громкости самоконтроля.
     * \param dB - громкость, дБ.
     */
    void monVolumeChanged(double dB);

    /**
     * \brief Уведомляет об изменении режимыа работы АРУ приёмника.
     * \param transceiver - номер трансивера.
     * \param mode - режим работы АРУ приёмника.
     */
    void agcModeChanged(quint32 transceiver, TciClient::AgcMode mode);

    /**
     * \brief Уведомляет об изменении усиления АРУ приёмника.
     * \param transceiver - номер трансивера.
     * \param dB - усиление, дБ.
     */
    void agcGainChanged(quint32 transceiver, double dB);

    /**
     * \brief Уведомляет об изменении флага включения фильтра NB приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void nbEnableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении параметров фильтра NB приёмника.
     * \param transceiver - номер трансивера.
     * \param threshold - флаг включения.
     * \param time - флаг включения.
     */
    void nbParamChanged(quint32 transceiver, double threshold, double time);

    /**
     * \brief Уведомляет об изменении флага включения псевдостерео приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void binauralEnableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения фильтра NR приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void nrEnableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения фильтра ANC приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void ancEnableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения фильтра ANF приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void anfEnableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения фильтра APF приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void apfEnableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения фильтра DSE приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void dseEnableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения фильтров NF приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void nfEnableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага блокировки частоты настройки.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void lockChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении флага включения порогового шумодава.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void squelchEnableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об изменении порога открытия шумодава.
     * \param transceiver - номер трансивера.
     * \param dB - порог открытия.
     */
    void squelchThresholdChanged(quint32 transceiver, double dB);

    /**
     * \brief Уведомляет об изменении смещения частоты.
     * \param Hz - смещение частоты, Гц.
     */
    void diglOffsetChanged(double Hz);

    /**
     * \brief Уведомляет об изменении смещения частоты.
     * \param Hz - смещение частоты, Гц.
     */
    void diguOffsetChanged(double Hz);

    /**
     * \brief Уведомляет об изменении флага разрешения передачи.
     * \param transceiver - номер трансивера.
     * \param enable - флаг разрешения передачи.
     */
    void txAvaliableChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет об запуске IQ потока.
     * \param transceiver - номер трансивера.
     */
    void iqStarted(quint32 transceiver);

    /**
     * \brief Уведомляет об остановке IQ потока.
     * \param transceiver - номер трансивера.
     */
    void iqStoped(quint32 transceiver);

    /**
     * \brief Уведомляет об запуске аудио потока.
     * \param transceiver - номер трансивера.
     */
    void audioStarted(quint32 transceiver);

    /**
     * \brief Уведомляет об остановке аудио потока.
     * \param transceiver - номер трансивера.
     */
    void audioStoped(quint32 transceiver);

    /**
     * \brief Уведомляет об запуске потока линейного аудио выхода.
     * \param transceiver - номер трансивера.
     */
    void lineoutStarted(quint32 transceiver);

    /**
     * \brief Уведомляет об остановке потока линейного аудио выхода.
     * \param transceiver - номер трансивера.
     */
    void lineoutStoped(quint32 transceiver);

    /**
     * \brief Уведомляет о клике мышью по споту на панораме.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param callsign - позывной.
     * \param Hz - частота.
     */
    void clickedOnSpots(quint32 transceiver, quint32 channel, QString callsign, double Hz);

    /**
     * \brief Сигнал нажатия на педаль PTT.
     * \param transceiver - номер трансивера.
     * \param enable - флаг нажатия на педаль.
     */
    void txFootSwitchChanged(quint32 transceiver, bool enable);

    /**
     * \brief Уведомляет о текущей частоте передатчика.
     * \param Hz - частота передачи.
     */
    void txFrequencyChanged(double Hz);

    /**
     * \brief Уведомляет об изменении фокуса программы.
     * \param focus - в фокусе - \a true, не в фокусе - \a false
     */
    void appFocusChanged(bool focus);

    /**
     * \brief Показания мощности сигнала в полосе приёма.
     * \param transceiver - номер трансивера.
     * \param dBm - мощность сигнала.
     */
    void rxSensors(quint32 transceiver, double dBm);

    /**
     * \brief Показания мощности сигнала в полосе приёма.
     * \param transceiver - номер трансивера.
     * \param mic - мощность микрофонного сигнала.
     * \param rms - мощность сигнала на выходе передатчика, Вт (RMS).
     * \param peak - пиковая мощность сигнала на выходе передатчика, Вт.
     * \param swr - КСВ передатчика.
     */
    void txSensors(quint32 transceiver, double mic, double rms, double peak, double swr);


public slots:
    /**
     * \brief Подключение к TCI серверу.
     * \param address - адрес сервера (ws://localhost:50001)
     */
    void connect(const QString &address);

    /**
     * \brief Отключение от TCI сервера.
     */
    void disconnect();

    /**
     * \brief Запуск устройства.
     *
     * \see stop()
     * \see started()
     * \see stoped()
     */
    void start();

    /**
     * \brief Остановка устройства.
     *
     * \see start()
     * \see started()
     * \see stoped()
     */
    void stop();

    /**
     * \brief Установка центральной частоты, Гц.
     * \param transceiver - номер трансивера.
     * \param Hz - центральная частота, Гц.
     */
    void setDds(quint32 transceiver, double Hz);

    /**
     * \brief Установка частоты настройки ПЧ канала трансивера, Гц.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param Hz - частота, Гц.
     */
    void setIf(quint32 transceiver, quint32 channel, double Hz);

    /**
     * \brief Установка частоты настройки канала трансивера, Гц.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param Hz - частота, Гц.
     */
    void setVfo(quint32 transceiver, quint32 channel, double Hz);

    /**
     * \brief Установка вида связи трансиверу.
     * \param transceiver - номер трансивера.
     * \param modulation - модуляция.
     */
    void setModulation(quint32 transceiver, const QString &modulation);

    /**
     * \brief Установка режима работы трансиверу.
     * \param transceiver - номер трансивера.
     * \param enable - если \a true - передача, иначе режим приёма.
     * \param opt - опциональный параметр.
     * \see trxChanged
     */
    void setTrx(quint32 transceiver, bool enable, const QString &opt);

    /**
     * \brief Установка режима работы трансиверу.
     * \param transceiver - номер трансивера.
     * \param enable - если \a true - излучение несущего колебания, иначе режим приёма.
     * \see tuneChanged
     */
    void setTune(quint32 transceiver, bool enable);

    /**
     * \brief Установка мощности передатчику.
     * \param transceiver - номер трансивера.
     * \param value - значение мощности передатчика.
     * \details Значение мощности принимает значения от 0 до 100.
     * \see driveChanged
     */
    void setDrive(quint32 transceiver, double value);

    /**
     * \brief Установка мощности передатчику в режиме излучения несущей .
     * \param transceiver - номер трансивера.
     * \param value - значение мощности передатчика.
     * \details Значение мощности принимает значения от 0 до 100.
     * \see driveChanged
     */
    void setTuneDrive(quint32 transceiver, double value);

    /**
     * \brief Включает/Выключает расстройку частоты приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setRitEnable(quint32 transceiver, bool enable);

    /**
     * \brief Включает/Выключает расстройку частоты передатчика.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setXitEnable(quint32 transceiver, bool enable);

    /**
     * \brief Включает/Выключает функцию SPLIT.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setSplitEnable(quint32 transceiver, bool enable);

    /**
     * \brief Установка частоты расстройки приёмника.
     * \param transceiver - номер трансивера.
     * \param Hz - частота расстройки.
     */
    void setRitOffset(quint32 transceiver, double Hz);

    /**
     * \brief Установка частоты расстройки передатчика.
     * \param transceiver - номер трансивера.
     * \param Hz - частота расстройки.
     */
    void setXitOffset(quint32 transceiver, double Hz);

    /**
     * \brief Установка флага включения программного трансивера.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     * \details Первый трансивер всегда включён.
     */
    void setTransceiverEnable(quint32 transceiver, bool enable);

    /**
     * \brief Установка флага включения дополнительных каналов приёма.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param enable - флаг включения.
     * \details Первый канала всегда включён.
     */
    void setRxChannelEnable(quint32 transceiver, quint32 channel, bool enable);

    /**
     * \brief Установка полосы приёмного фильтра.
     * \param transceiver - номер трансивера.
     * \param low - номер канала.
     * \param high - флаг включения.
     */
    void setRxFilterBand(quint32 transceiver, double low, double high);

    /**
     * \brief Установка скорости телеграфирования макроса.
     * \param wpm - скорость телеграфирования.
     */
    void setCwMacrosSpeed(quint32 wpm);

    /**
     * \brief Установка задержки перед началом передачи макроса.
     * \param ms - задержка перед началом передачи макроса.
     */
    void setCwMacrosDelay(quint32 ms);

    /**
     * \brief Увеличение скорости телеграфирования макроса относительно установленной скорости.
     * \param wpm - значение на которое увеличится устанволенная скорость.
     */
    void setCwMacrosSpeedUp(quint32 wpm);

    /**
     * \brief Уменьшение скорости телеграфирования макроса относительно установленной скорости.
     * \param wpm - значение на которое уменьшится устанволенная скорость.
     */
    void setCwMacrosSpeedDown(quint32 wpm);

    /**
     * \brief Установка скорости телеграфирования ключом.
     * \param wpm - скорость телеграфирования.
     */
    void setCwKeyerSpeed(quint32 wpm);

    /**
     * \brief Установка общей громкости.
     * \param dB - громкость, дБ.
     */
    void setVolume(double dB);

    /**
     * \brief Установка громкости каналу приёмника.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param dB - громкость, дБ.
     */
    void setRxVolume(quint32 transceiver, quint32 channel, double dB);

    /**
     * \brief Установка баланса громкости канала приёмника.
     * \param transceiver - номер трансивера.
     * \param channel - номер канала.
     * \param dB - громкость, дБ.
     */
    void setRxBalance(quint32 transceiver, quint32 channel, double dB);

    /**
     * \brief Выключение общей громкости.
     * \param enable - флаг выключения.
     */
    void setMute(bool enable);

    /**
     * \brief Выключение громкости приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг выключения.
     */
    void setRxMute(quint32 transceiver, bool enable);

    /**
     * \brief Установка флага включения самоконтроля.
     * \param enable - флаг включения.
     */
    void setMonEnable(bool enable);

    /**
     * \brief Установка громкости самоконтроля.
     * \param dB - громкость, дБ.
     */
    void setMonVolume(double dB);

    /**
     * \brief Установка режима работы АРУ приёмника.
     * \param transceiver - номер трансивера.
     * \param mode - режим работы АРУ приёмника.
     */
    void setAgcMode(quint32 transceiver, TciClient::AgcMode mode);

    /**
     * \brief Установка усиления АРУ приёмника.
     * \param transceiver - номер трансивера.
     * \param dB - усиление, дБ.
     */
    void setAgcGain(quint32 transceiver, double dB);

    /**
     * \brief Включение/Выключение фильтра NB приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setNbEnable(quint32 transceiver, bool enable);

    /**
     * \brief Установка параметров фильтру NB приёмника.
     * \param transceiver - номер трансивера.
     * \param threshold - флаг включения.
     * \param time - флаг включения.
     */
    void setNbParam(quint32 transceiver, double threshold, double time);

    /**
     * \brief Включение/Выключение псевдостерео приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setBinauralEnable(quint32 transceiver, bool enable);

    /**
     * \brief Включение/Выключение фильтра NR приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setNrEnable(quint32 transceiver, bool enable);

    /**
     * \brief Включение/Выключение фильтра ANC приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setAncEnable(quint32 transceiver, bool enable);

    /**
     * \brief Включение/Выключение фильтра APF приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setApfEnable(quint32 transceiver, bool enable);

    /**
     * \brief Включение/Выключение фильтра DSE приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setDseEnable(quint32 transceiver, bool enable);

    /**
     * \brief Включение/Выключение фильтров NF приёмника.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setNfEnable(quint32 transceiver, bool enable);

    /**
     * \brief Включение/Выключение блокировки частоты настройки.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setLock(quint32 transceiver, bool enable);

    /**
     * \brief Включение/Выключение порогового шумодава.
     * \param transceiver - номер трансивера.
     * \param enable - флаг включения.
     */
    void setSquelchEnable(quint32 transceiver, bool enable);

    /**
     * \brief Установка порога открытия шумодава.
     * \param transceiver - номер трансивера.
     * \param dB - порог открытия.
     */
    void setSquelchThreshold(quint32 transceiver, double dB);

    /**
     * \brief Установка смещения частоты.
     * \param Hz - смещение частоты, гц.
     */
    void setDiglOffset(double Hz);

    /**
     * \brief Установка смещения частоты.
     * \param Hz - смещение частоты, гц.
     */
    void setDiguOffset(double Hz);

    /**
     * \brief Установка спота.
     * \param callsign - позывной.
     * \param modulation - вид связи.
     * \param Hz - частота спота.
     * \param color - цвет отображения.
     * \param text - дополнительный текст (необязательный параметр).
     */
    void setSpot(const QString &callsign, const QString &modulation, double Hz, uint32_t argb, const QString &text);

    /**
     * \brief Удаление спота.
     * \param callsign - позывной.
     */
    void deleteSpot(const QString &callsign);

    /**
     * \brief Удаление всех спотов.
     */
    void clearSpot();

    /**
     * \brief Установка количества сэмплов в одном фрэйме.
     * \param samples - количество сэмплов.
     */
    void setAudioSamplesPerFrame(quint32 samples);

    /**
     * \brief Запуск IQ потока.
     * \param transceiver - номер трансивера.
     */
    void iqStart(quint32 transceiver);

    /**
     * \brief Остановка IQ потока.
     * \param transceiver - номер трансивера.
     */
    void iqStop(quint32 transceiver);

    /**
     * \brief Запуск аудио потока.
     * \param transceiver - номер трансивера.
     */
    void audioStart(quint32 transceiver);

    /**
     * \brief Остановка аудио потока.
     * \param transceiver - номер трансивера.
     */
    void audioStop(quint32 transceiver);

    /**
     * \brief Запуск потока линейного аудио выхода.
     * \param transceiver - номер трансивера.
     */
    void lineoutStart(quint32 transceiver);

    /**
     * \brief Остановка потока линейного аудио выхода.
     * \param transceiver - номер трансивера.
     */
    void lineoutStop(quint32 transceiver);

    /**
     * \brief Начать запись аудиопотока из линейного аудио выхода.
     * \param transceiver - номер трансивера.
     * \param sec - максимальное время записи.
     */
    void lineoutRecordStart(quint32 transceiver, quint32 sec);

    /**
     * \brief Сохранить запись аудиопотока из линейного аудио выхода в файл.
     * \param transceiver - номер трансивера.
     * \param fileName - полное имя файла.
     */
    void lineoutRecordSave(quint32 transceiver, const QString &fileName);

    /**
     * \brief Прекратить запись аудиопотока из линейного аудио выхода и удалить накопленную запись.
     * \param transceiver - номер трансивера.
     */
    void lineoutRecordBreak(quint32 transceiver);

    /**
     * \brief Сделать главное окно программы активным (в фокусе).
     */
    void setAppInFocus();

    /**
     * \brief Включение/Выключение отправки уровня сигнала в полосе приёма.
     * \param enable - флаг включения отправки.
     * \param ms - интервал отправки.
     */
    void setRxSensorsEnable(bool enable, quint32 ms);

    /**
     * \brief Включение/Выключение отправки показаний измерителей передатчика.
     * \param enable - флаг включения отправки.
     * \param ms - интервал отправки.
     */
    void setTxSensorsEnable(bool enable, quint32 ms);

private:
    struct impl;
    std::unique_ptr<impl> pImpl;
};


