#include "TciClient.h"
#include "serialize/TciSerialize.h"
#include <QUrl>
#include <QWebSocket>
#include <utility>

struct TciClient::impl final {
    explicit impl(TciClient& context)
        : m_context { context }
    {
        m_executors[ee::tci::Command::Protocol] = [&](std::string_view text) { onProtocol(text); };
        m_executors[ee::tci::Command::Device] = [&](std::string_view text) { onDeviceName(text); };
        m_executors[ee::tci::Command::ReceiveOnly] = [&](std::string_view text) { onReceiveOnly(text); };
        m_executors[ee::tci::Command::TrxCount] = [&](std::string_view text) { onTrxCount(text); };
        m_executors[ee::tci::Command::ChannelsCount] = [&](std::string_view text) { onChannelsCount(text); };
        m_executors[ee::tci::Command::VfoLimits] = [&](std::string_view text) { onVfoLimits(text); };
        m_executors[ee::tci::Command::IfLimits] = [&](std::string_view text) { onIfLimits(text); };
        m_executors[ee::tci::Command::ModulationsList] = [&](std::string_view text) { onModulations(text); };
        m_executors[ee::tci::Command::Ready] = [&](std::string_view text) { onReady(text); };
        m_executors[ee::tci::Command::Start] = [&](std::string_view text) { onStart(text); };
        m_executors[ee::tci::Command::Stop] = [&](std::string_view text) { onStop(text); };
        m_executors[ee::tci::Command::Dds] = [&](std::string_view text) { onDds(text); };
        m_executors[ee::tci::Command::If] = [&](std::string_view text) { onHeterodyne(text); };
        m_executors[ee::tci::Command::Vfo] = [&](std::string_view text) { onVfo(text); };
        m_executors[ee::tci::Command::Modulation] = [&](std::string_view text) { onModulation(text); };
        m_executors[ee::tci::Command::Trx] = [&](std::string_view text) { onTrx(text); };
        m_executors[ee::tci::Command::Tune] = [&](std::string_view text) { onTune(text); };
        m_executors[ee::tci::Command::Drive] = [&](std::string_view text) { onDrive(text); };
        m_executors[ee::tci::Command::TuneDrive] = [&](std::string_view text) { onTuneDrive(text); };
        m_executors[ee::tci::Command::RitEnable] = [&](std::string_view text) { onRitEnabled(text); };
        m_executors[ee::tci::Command::XitEnable] = [&](std::string_view text) { onXitEnabled(text); };
        m_executors[ee::tci::Command::SplitEnable] = [&](std::string_view text) { onSplitEnabled(text); };
        m_executors[ee::tci::Command::RitOffset] = [&](std::string_view text) { onRitOffset(text); };
        m_executors[ee::tci::Command::XitOffset] = [&](std::string_view text) { onXitOffset(text); };
        m_executors[ee::tci::Command::RxEnable] = [&](std::string_view text) { onTransceiverEnabled(text); };
        m_executors[ee::tci::Command::RxChannelEnable] = [&](std::string_view text) { onRxChannelEnabled(text); };
        m_executors[ee::tci::Command::RxFilterBand] = [&](std::string_view text) { onRxFilterBand(text); };
        m_executors[ee::tci::Command::CwMacrosSpeed] = [&](std::string_view text) { onCwMacrosSpeed(text); };
        m_executors[ee::tci::Command::CwMacrosDelay] = [&](std::string_view text) { onCwMacrosDelay(text); };
        m_executors[ee::tci::Command::CwKeyerSpeed] = [&](std::string_view text) { onCwKeyerSpeed(text); };
        m_executors[ee::tci::Command::Volume] = [&](std::string_view text) { onVolume(text); };
        m_executors[ee::tci::Command::RxVolume] = [&](std::string_view text) { onRxVolume(text); };
        m_executors[ee::tci::Command::RxBalance] = [&](std::string_view text) { onRxBalance(text); };
        m_executors[ee::tci::Command::Mute] = [&](std::string_view text) { onMute(text); };
        m_executors[ee::tci::Command::RxMute] = [&](std::string_view text) { onRxMute(text); };
        m_executors[ee::tci::Command::MonEnable] = [&](std::string_view text) { onMonEnable(text); };
        m_executors[ee::tci::Command::MonVolume] = [&](std::string_view text) { onMonVolume(text); };
        m_executors[ee::tci::Command::AgcMode] = [&](std::string_view text) { onAgcMode(text); };
        m_executors[ee::tci::Command::AgcGain] = [&](std::string_view text) { onAgcGain(text); };
        m_executors[ee::tci::Command::RxNbEnable] = [&](std::string_view text) { onRxNbEnabled(text); };
        m_executors[ee::tci::Command::RxNbParams] = [&](std::string_view text) { onRxNbParam(text); };
        m_executors[ee::tci::Command::RxBin] = [&](std::string_view text) { onRxBinEnabled(text); };
        m_executors[ee::tci::Command::RxNr] = [&](std::string_view text) { onRxNrEnabled(text); };
        m_executors[ee::tci::Command::RxAnc] = [&](std::string_view text) { onRxAncEnabled(text); };
        m_executors[ee::tci::Command::RxAnf] = [&](std::string_view text) { onRxAnfEnabled(text); };
        m_executors[ee::tci::Command::RxApf] = [&](std::string_view text) { onRxApfEnabled(text); };
        m_executors[ee::tci::Command::RxDse] = [&](std::string_view text) { onRxDseEnabled(text); };
        m_executors[ee::tci::Command::RxNf] = [&](std::string_view text) { onRxNfEnabled(text); };
        m_executors[ee::tci::Command::Lock] = [&](std::string_view text) { onLock(text); };
        m_executors[ee::tci::Command::SqlEnable] = [&](std::string_view text) { onSquelch(text); };
        m_executors[ee::tci::Command::SqlLevel] = [&](std::string_view text) { onSquelchThreshold(text); };
        m_executors[ee::tci::Command::DiglOffset] = [&](std::string_view text) { onDiglOffset(text); };
        m_executors[ee::tci::Command::DiguOffset] = [&](std::string_view text) { onDiguOffset(text); };
        m_executors[ee::tci::Command::TxEnable] = [&](std::string_view text) { onTrxAvaliable(text); };
        m_executors[ee::tci::Command::IqStart] = [&](std::string_view text) { onIqStart(text); };
        m_executors[ee::tci::Command::IqStop] = [&](std::string_view text) { onIqStop(text); };
        m_executors[ee::tci::Command::AudioStart] = [&](std::string_view text) { onAudioStart(text); };
        m_executors[ee::tci::Command::AudioStop] = [&](std::string_view text) { onAudioStop(text); };
        m_executors[ee::tci::Command::LineOutStart] = [&](std::string_view text) { onLineOutStart(text); };
        m_executors[ee::tci::Command::LineOutStop] = [&](std::string_view text) { onLineoutStop(text); };
        m_executors[ee::tci::Command::RxClickedOnSpot] = [&](std::string_view text) { onClickedOnSpot(text); };
        m_executors[ee::tci::Command::TxFootswitch] = [&](std::string_view text) { onTxFootSwitch(text); };
        m_executors[ee::tci::Command::TxFrequency] = [&](std::string_view text) { onTxFrequency(text); };
        m_executors[ee::tci::Command::AppFocus] = [&](std::string_view text) { onAppFocus(text); };
        m_executors[ee::tci::Command::RxSensors] = [&](std::string_view text) { onRxSensors(text); };
        m_executors[ee::tci::Command::TxSensors] = [&](std::string_view text) { onTxSensors(text); };

        QObject::connect(&m_socket, &QWebSocket::connected, &m_socket, [&]() { onConnected(); });
        QObject::connect(&m_socket, &QWebSocket::disconnected, &m_socket, [&]() { onDisconnected(); });
        QObject::connect(&m_socket, &QWebSocket::textMessageReceived, &m_socket, [&](const auto& text) { onText(text); });

        pCurrentState->onEnter();
    }

    ~impl()
    {
        disconnect();
    }

    [[nodiscard]] TciClient::Status status() const noexcept
    {
        return m_status;
    }

    void connect(const QString& address)
    {
        pCurrentState->connect(address);
    }

    void disconnect()
    {
        pCurrentState->disconnect();
    }

    void start()
    {
        send(ee::tci::serialize(ee::tci::Command::Start));
    }

    void stop()
    {
        send(ee::tci::serialize(ee::tci::Command::Stop));
    }

    void setDds(quint32 transceiver, double Hz)
    {
        send(ee::tci::serialize(ee::tci::Command::Dds, transceiver, int64_t(Hz)));
    }

    void setIf(quint32 transceiver, quint32 channel, double Hz)
    {
        send(ee::tci::serialize(ee::tci::Command::If, transceiver, channel, int64_t(Hz)));
    }

    void setVfo(quint32 transceiver, quint32 channel, double Hz)
    {
        send(ee::tci::serialize(ee::tci::Command::Vfo, transceiver, channel, int64_t(Hz)));
    }

    void setModulation(quint32 transceiver, const QString& modulation)
    {
        send(ee::tci::serialize(ee::tci::Command::Modulation, transceiver, modulation.toStdString()));
    }

    void setTrx(quint32 transceiver, bool enable, const QString& opt)
    {
        if (opt.isEmpty())
            send(ee::tci::serialize(ee::tci::Command::Trx, transceiver, enable));
        else
            send(ee::tci::serialize(ee::tci::Command::Trx, transceiver, opt.toStdString()));
    }

    void setTune(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::Tune, transceiver, enable));
    }

    void setDrive(quint32 transceiver, double value)
    {
        send(ee::tci::serialize(ee::tci::Command::Drive, transceiver, value));
    }

    void setTuneDrive(quint32 transceiver, double value)
    {
        send(ee::tci::serialize(ee::tci::Command::TuneDrive, transceiver, value));
    }

    void setRitEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::RitEnable, transceiver, enable));
    }

    void setXitEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::XitEnable, transceiver, enable));
    }

    void setSplitEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::SplitEnable, transceiver, enable));
    }

    void setRitOffset(quint32 transceiver, double Hz)
    {
        send(ee::tci::serialize(ee::tci::Command::RitOffset, transceiver, int64_t(Hz)));
    }

    void setXitOffset(quint32 transceiver, double Hz)
    {
        send(ee::tci::serialize(ee::tci::Command::XitOffset, transceiver, int64_t(Hz)));
    }

    void setTransceiverEnable(quint32 transceiver, bool enable)
    {
        if (transceiver != 0)
            send(ee::tci::serialize(ee::tci::Command::RxEnable, transceiver, enable));
    }

    void setRxChannelEnable(quint32 transceiver, quint32 channel, bool enable)
    {
        if (channel != 0)
            send(ee::tci::serialize(ee::tci::Command::RxChannelEnable, transceiver, channel, enable));
    }

    void setRxFilterBand(quint32 transceiver, double low, double high)
    {
        send(ee::tci::serialize(ee::tci::Command::RxFilterBand, transceiver, low, high));
    }

    void setCwMacrosSpeed(quint32 wpm)
    {
        send(ee::tci::serialize(ee::tci::Command::CwMacrosSpeed, wpm));
    }

    void setCwMacrosDelay(quint32 ms)
    {
        send(ee::tci::serialize(ee::tci::Command::CwMacrosDelay, ms));
    }

    void setCwMacrosSpeedUp(quint32 wpm)
    {
        send(ee::tci::serialize(ee::tci::Command::CwMacrosSpeedUp, wpm));
    }

    void setCwMacrosSpeedDown(quint32 wpm)
    {
        send(ee::tci::serialize(ee::tci::Command::CwMacrosSpeedDown, wpm));
    }

    void setCwKeyerSpeed(quint32 wpm)
    {
        send(ee::tci::serialize(ee::tci::Command::CwKeyerSpeed, wpm));
    }

    void setVolume(double dB)
    {
        send(ee::tci::serialize(ee::tci::Command::Volume, dB));
    }

    void setRxVolume(quint32 transceiver, quint32 channel, double dB)
    {
        send(ee::tci::serialize(ee::tci::Command::RxVolume, transceiver, channel, dB));
    }

    void setRxBalance(quint32 transceiver, quint32 channel, double dB)
    {
        send(ee::tci::serialize(ee::tci::Command::RxBalance, transceiver, channel, dB));
    }

    void setMute(bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::Mute, enable));
    }

    void setRxMute(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::RxMute, transceiver, enable));
    }

    void setMonEnable(bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::MonEnable, enable));
    }

    void setMonVolume(double dB)
    {
        send(ee::tci::serialize(ee::tci::Command::MonVolume, dB));
    }

    void setAgcMode(quint32 transceiver, TciClient::AgcMode mode)
    {
        if (mode == TciClient::AgcMode::Off)
            send(ee::tci::serialize(ee::tci::Command::AgcMode, transceiver, "off"));
        else if (mode == TciClient::AgcMode::Fast)
            send(ee::tci::serialize(ee::tci::Command::AgcMode, transceiver, "fast"));
        else if (mode == TciClient::AgcMode::Normal)
            send(ee::tci::serialize(ee::tci::Command::AgcMode, transceiver, "normal"));
    }

    void setAgcGain(quint32 transceiver, double dB)
    {
        send(ee::tci::serialize(ee::tci::Command::AgcGain, transceiver, dB));
    }

    void setNbEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::RxNbEnable, transceiver, enable));
    }

    void setNbParam(quint32 transceiver, double threshold, double time)
    {
        send(ee::tci::serialize(ee::tci::Command::RxNbParams, transceiver, threshold, time));
    }

    void setBinauralEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::RxBin, transceiver, enable));
    }

    void setNrEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::RxNr, transceiver, enable));
    }

    void setAncEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::RxAnc, transceiver, enable));
    }

    void setApfEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::RxApf, transceiver, enable));
    }

    void setDseEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::RxDse, transceiver, enable));
    }

    void setNfEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::RxNf, transceiver, enable));
    }

    void setLock(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::Lock, transceiver, enable));
    }

    void setSquelchEnable(quint32 transceiver, bool enable)
    {
        send(ee::tci::serialize(ee::tci::Command::SqlEnable, transceiver, enable));
    }

    void setSquelchThreshold(quint32 transceiver, double dB)
    {
        send(ee::tci::serialize(ee::tci::Command::SqlLevel, transceiver, dB));
    }

    void setDiglOffset(double Hz)
    {
        send(ee::tci::serialize(ee::tci::Command::DiglOffset, int64_t(Hz)));
    }

    void setDiguOffset(double Hz)
    {
        send(ee::tci::serialize(ee::tci::Command::DiguOffset, int64_t(Hz)));
    }

    void setSpot(const QString& callsign, const QString& modulation, double Hz, uint32_t argb, const QString& text)
    {
        send(ee::tci::serialize(ee::tci::Command::Spot,
            callsign.toStdString(),
            modulation.toStdString(),
            int64_t(Hz),
            argb,
            text.toStdString()));
    }

    void deleteSpot(const QString& callsign)
    {
        send(ee::tci::serialize(ee::tci::Command::SpotDelete, callsign.toStdString()));
    }

    void clearSpot()
    {
        send(ee::tci::serialize(ee::tci::Command::SpotClear));
    }

    void setAudioSamplesPerFrame(quint32 samples)
    {
        send(ee::tci::serialize(ee::tci::Command::AudioStreamSamples, samples));
    }

    void iqStart(quint32 transceiver)
    {
        send(ee::tci::serialize(ee::tci::Command::IqStart, transceiver));
    }

    void iqStop(quint32 transceiver)
    {
        send(ee::tci::serialize(ee::tci::Command::IqStop, transceiver));
    }

    void audioStart(quint32 transceiver)
    {
        send(ee::tci::serialize(ee::tci::Command::AudioStart, transceiver));
    }

    void audioStop(quint32 transceiver)
    {
        send(ee::tci::serialize(ee::tci::Command::AudioStop, transceiver));
    }

    void lineoutStart(quint32 transceiver)
    {
        send(ee::tci::serialize(ee::tci::Command::LineOutStart, transceiver));
    }

    void lineoutStop(quint32 transceiver)
    {
        send(ee::tci::serialize(ee::tci::Command::LineOutStop, transceiver));
    }

    void lineoutRecordStart(quint32 transceiver, quint32 sec)
    {
        send(ee::tci::serialize(ee::tci::Command::LineOutRecordStart, transceiver, sec));
    }

    void lineoutRecordSave(quint32 transceiver, const QString& fileName)
    {
        send(ee::tci::serialize(ee::tci::Command::LineOutRecordSave, transceiver, fileName.toStdString()));
    }

    void lineoutRecordBreak(quint32 transceiver)
    {
        send(ee::tci::serialize(ee::tci::Command::LineOutRecordBreak, transceiver));
    }

    void setAppInFocus()
    {
        send(ee::tci::serialize(ee::tci::Command::SetInFocus));
    }

    void setRxSensorsEnable(bool enable, quint32 ms)
    {
        send(ee::tci::serialize(ee::tci::Command::RxSensorsEnable, enable, ms));
    }

    void setTxSensorsEnable(bool enable, quint32 ms)
    {
        send(ee::tci::serialize(ee::tci::Command::TxSensorsEnable, enable, ms));
    }

private:
    void onText(const QString& text)
    {
        const auto t_commands = text.toLower().toStdString();
        // разбиваем список команд на отдельные команды
        for (std::string_view command : ee::tci::split(t_commands, ee::tci::end_cmd)) {
            // обрабатываем команды по очереди
            try {
                // выполняем команду
                m_executors[ee::tci::get_cmd(command)](command);
            } catch (...) { /* игнорируем ошибку */
            }
        }
    }

    void send(const std::string& text)
    {
        m_socket.sendTextMessage(QString::fromStdString(text));
    }

    void onConnected()
    {
        pCurrentState->onConnected();
    }

    void onDisconnected()
    {
        pCurrentState->onDisconnected();
    }

private:
    void onProtocol(std::string_view text)
    {
        try {
            if (const auto [name, version] = std::get<1>(ee::tci::deserialize<std::string, std::string>(text)); name && version)
                emit m_context.protocolChanged(QString::fromStdString(*name), QString::fromStdString(*version));
        } catch (...) {
        }
    }

    void onDeviceName(std::string_view text)
    {
        try {
            if (const auto [name] = std::get<1>(ee::tci::deserialize<std::string>(text)); name)
                emit m_context.deviceNameChanged(QString::fromStdString(*name));
        } catch (...) {
        }
    }

    void onReceiveOnly(std::string_view text)
    {
        try {
            if (const auto [enable] = std::get<1>(ee::tci::deserialize<bool>(text)); enable)
                emit m_context.receiveOnlyChanged(*enable);
        } catch (...) {
        }
    }

    void onTrxCount(std::string_view text)
    {
        try {
            if (const auto [value] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); value)
                emit m_context.trxCountChanged(*value);
        } catch (...) {
        }
    }

    void onChannelsCount(std::string_view text)
    {
        try {
            if (const auto [value] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); value)
                emit m_context.channelsCountChanged(*value);
        } catch (...) {
        }
    }

    void onVfoLimits(std::string_view text)
    {
        try {
            if (const auto [low, high] = std::get<1>(ee::tci::deserialize<double, double>(text)); low && high)
                emit m_context.vfoLimitsChanged(*low, *high);
        } catch (...) {
        }
    }

    void onIfLimits(std::string_view text)
    {
        try {
            if (const auto [low, high] = std::get<1>(ee::tci::deserialize<double, double>(text)); low && high)
                emit m_context.ifLimitsChanged(*low, *high);
        } catch (...) {
        }
    }

    void onModulations(std::string_view text)
    {
        if (const auto list = ee::tci::split(text, ':'); list.size() == 2) {
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            emit m_context.modulationsListChanged(QString::fromStdString(std::string { list.at(1) }).split(",", QString::SkipEmptyParts));
#else
            emit m_context.modulationsListChanged(QString::fromStdString(std::string { list.at(1) }).split(",", Qt::SkipEmptyParts));
#endif
        }
    }

    void onReady(std::string_view)
    {
        emit m_context.ready();
    }

    void onStart(std::string_view)
    {
        emit m_context.started();
    }

    void onStop(std::string_view)
    {
        emit m_context.stoped();
    }

    void onDds(std::string_view text)
    {
        try {
            if (const auto [transceiver, Hz] = std::get<1>(ee::tci::deserialize<uint32_t, double>(text)); transceiver && Hz)
                emit m_context.ddsChanged(*transceiver, *Hz);
        } catch (...) {
        }
    }

    void onHeterodyne(std::string_view text)
    {
        try {
            if (const auto [transceiver, channel, Hz] = std::get<1>(ee::tci::deserialize<uint32_t, uint32_t, double>(text)); transceiver && channel && Hz)
                emit m_context.ifChanged(*transceiver, *channel, *Hz);
        } catch (...) {
        }
    }

    void onVfo(std::string_view text)
    {
        try {
            if (const auto [transceiver, channel, Hz] = std::get<1>(ee::tci::deserialize<uint32_t, uint32_t, double>(text)); transceiver && channel && Hz)
                emit m_context.vfoChanged(*transceiver, *channel, *Hz);
        } catch (...) {
        }
    }

    void onModulation(std::string_view text)
    {
        try {
            if (const auto [transceiver, mode] = std::get<1>(ee::tci::deserialize<uint32_t, std::string>(text)); transceiver && mode)
                emit m_context.modulationChanged(*transceiver, QString::fromStdString({ *mode }));
        } catch (...) {
        }
    }

    void onTrx(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable, opt] = std::get<1>(ee::tci::deserialize<uint32_t, bool, std::string>(text)); transceiver && enable) {
                emit m_context.trxChanged(*transceiver, *enable);
            }
        } catch (...) {
        }
    }

    void onTune(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.tuneChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onDrive(std::string_view text)
    {
        try {
            if (const auto [transceiver, value] = std::get<1>(ee::tci::deserialize<uint32_t, double>(text)); transceiver && value)
                emit m_context.driveChanged(*transceiver, *value);
        } catch (...) {
        }
    }

    void onTuneDrive(std::string_view text)
    {
        try {
            if (const auto [transceiver, value] = std::get<1>(ee::tci::deserialize<uint32_t, double>(text)); transceiver && value)
                emit m_context.tuneDriveChanged(*transceiver, *value);
        } catch (...) {
        }
    }

    void onRitEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.ritEnabledChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onXitEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.xitEnabledChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onSplitEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.splitEnabledChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onRitOffset(std::string_view text)
    {
        try {
            if (const auto [transceiver, Hz] = std::get<1>(ee::tci::deserialize<uint32_t, double>(text)); transceiver && Hz)
                emit m_context.ritOffsetChanged(*transceiver, *Hz);
        } catch (...) {
        }
    }

    void onXitOffset(std::string_view text)
    {
        try {
            if (const auto [transceiver, Hz] = std::get<1>(ee::tci::deserialize<uint32_t, double>(text)); transceiver && Hz)
                emit m_context.xitOffsetChanged(*transceiver, *Hz);
        } catch (...) {
        }
    }

    void onTransceiverEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.transceiverEnabledChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onRxChannelEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, channel, enable] = std::get<1>(ee::tci::deserialize<uint32_t, uint32_t, bool>(text)); transceiver && channel && enable)
                emit m_context.rxChannelEnabledChanged(*transceiver, *channel, *enable);
        } catch (...) {
        }
    }

    void onRxFilterBand(std::string_view text)
    {
        try {
            if (const auto [transceiver, low, high] = std::get<1>(ee::tci::deserialize<uint32_t, double, double>(text)); transceiver && low && high)
                emit m_context.rxFilterBandChanged(*transceiver, *low, *high);
        } catch (...) {
        }
    }

    void onCwMacrosSpeed(std::string_view text)
    {
        try {
            if (const auto [wpm] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); wpm)
                emit m_context.cwMacrosSpeedChanged(*wpm);
        } catch (...) {
        }
    }

    void onCwMacrosDelay(std::string_view text)
    {
        try {
            if (const auto [ms] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); ms)
                emit m_context.cwMacrosDelayChanged(*ms);
        } catch (...) {
        }
    }

    void onCwKeyerSpeed(std::string_view text)
    {
        try {
            if (const auto [wpm] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); wpm)
                emit m_context.cwKeyerSpeedChanged(*wpm);
        } catch (...) {
        }
    }

    void onVolume(std::string_view text)
    {
        try {
            if (const auto [dB] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); dB)
                emit m_context.cwKeyerSpeedChanged(*dB);
        } catch (...) {
        }
    }

    void onRxVolume(std::string_view text)
    {
        try {
            if (const auto [transceiver, channel, dB] = std::get<1>(ee::tci::deserialize<uint32_t, uint32_t, double>(text)); transceiver && channel && dB)
                emit m_context.rxVolumeChanged(*transceiver, *channel, *dB);
        } catch (...) {
        }
    }

    void onRxBalance(std::string_view text)
    {
        try {
            if (const auto [transceiver, channel, dB] = std::get<1>(ee::tci::deserialize<uint32_t, uint32_t, double>(text)); transceiver && channel && dB)
                emit m_context.rxBalanceChanged(*transceiver, *channel, *dB);
        } catch (...) {
        }
    }

    void onMute(std::string_view text)
    {
        try {
            if (const auto [enable] = std::get<1>(ee::tci::deserialize<bool>(text)); enable)
                emit m_context.muteChanged(*enable);
        } catch (...) {
        }
    }

    void onRxMute(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.rxMuteChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onMonEnable(std::string_view text)
    {
        try {
            if (const auto [enable] = std::get<1>(ee::tci::deserialize<bool>(text)); enable)
                emit m_context.monEnableChanged(*enable);
        } catch (...) {
        }
    }

    void onMonVolume(std::string_view text)
    {
        try {
            if (const auto [dB] = std::get<1>(ee::tci::deserialize<double>(text)); dB)
                emit m_context.monVolumeChanged(*dB);
        } catch (...) {
        }
    }

    void onAgcMode(std::string_view text)
    {
        try {
            if (const auto [transceiver, mode] = std::get<1>(ee::tci::deserialize<uint32_t, std::string>(text)); transceiver && mode) {
                if (*mode == "off")
                    emit m_context.agcModeChanged(*transceiver, AgcMode::Off);
                else if (*mode == "fast")
                    emit m_context.agcModeChanged(*transceiver, AgcMode::Fast);
                else if (*mode == "normal")
                    emit m_context.agcModeChanged(*transceiver, AgcMode::Normal);
            }
        } catch (...) {
        }
    }

    void onAgcGain(std::string_view text)
    {
        try {
            if (const auto [transceiver, dB] = std::get<1>(ee::tci::deserialize<uint32_t, double>(text)); transceiver && dB)
                emit m_context.agcGainChanged(*transceiver, *dB);
        } catch (...) {
        }
    }

    void onRxNbEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.nbEnableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onRxNbParam(std::string_view text)
    {
        try {
            if (const auto [transceiver, threshold, time] = std::get<1>(ee::tci::deserialize<uint32_t, double, double>(text)); transceiver && threshold && time)
                emit m_context.nbParamChanged(*transceiver, *threshold, *time);
        } catch (...) {
        }
    }

    void onRxBinEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.binauralEnableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onRxNrEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.nrEnableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onRxAncEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.ancEnableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onRxAnfEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.anfEnableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onRxApfEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.apfEnableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onRxDseEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.dseEnableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onRxNfEnabled(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.nfEnableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onLock(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.lockChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onSquelch(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.squelchEnableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onSquelchThreshold(std::string_view text)
    {
        try {
            if (const auto [transceiver, threshold] = std::get<1>(ee::tci::deserialize<uint32_t, double>(text)); transceiver && threshold)
                emit m_context.squelchThresholdChanged(*transceiver, *threshold);
        } catch (...) {
        }
    }

    void onDiglOffset(std::string_view text)
    {
        try {
            if (const auto [Hz] = std::get<1>(ee::tci::deserialize<double>(text)); Hz)
                emit m_context.diglOffsetChanged(*Hz);
        } catch (...) {
        }
    }

    void onDiguOffset(std::string_view text)
    {
        try {
            if (const auto [Hz] = std::get<1>(ee::tci::deserialize<double>(text)); Hz)
                emit m_context.diguOffsetChanged(*Hz);
        } catch (...) {
        }
    }

    void onTrxAvaliable(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.txAvaliableChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onIqStart(std::string_view text)
    {
        try {
            if (const auto [transceiver] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); transceiver)
                emit m_context.iqStarted(*transceiver);
        } catch (...) {
        }
    }

    void onIqStop(std::string_view text)
    {
        try {
            if (const auto [transceiver] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); transceiver)
                emit m_context.iqStoped(*transceiver);
        } catch (...) {
        }
    }

    void onAudioStart(std::string_view text)
    {
        try {
            if (const auto [transceiver] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); transceiver)
                emit m_context.audioStarted(*transceiver);
        } catch (...) {
        }
    }

    void onAudioStop(std::string_view text)
    {
        try {
            if (const auto [transceiver] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); transceiver)
                emit m_context.audioStoped(*transceiver);
        } catch (...) {
        }
    }

    void onLineOutStart(std::string_view text)
    {
        try {
            if (const auto [transceiver] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); transceiver)
                emit m_context.lineoutStarted(*transceiver);
        } catch (...) {
        }
    }

    void onLineoutStop(std::string_view text)
    {
        try {
            if (const auto [transceiver] = std::get<1>(ee::tci::deserialize<uint32_t>(text)); transceiver)
                emit m_context.lineoutStoped(*transceiver);
        } catch (...) {
        }
    }

    void onClickedOnSpot(std::string_view text)
    {
        try {
            if (const auto [transceiver, channel, callsign, Hz] = std::get<1>(ee::tci::deserialize<uint32_t, uint32_t, std::string, double>(text)); transceiver && channel && callsign && Hz)
                emit m_context.clickedOnSpots(*transceiver, *channel, QString::fromStdString({ *callsign }), *Hz);
        } catch (...) {
        }
    }

    void onTxFootSwitch(std::string_view text)
    {
        try {
            if (const auto [transceiver, enable] = std::get<1>(ee::tci::deserialize<uint32_t, bool>(text)); transceiver && enable)
                emit m_context.txFootSwitchChanged(*transceiver, *enable);
        } catch (...) {
        }
    }

    void onTxFrequency(std::string_view text)
    {
        try {
            if (const auto [Hz] = std::get<1>(ee::tci::deserialize<double>(text)); Hz)
                emit m_context.txFrequencyChanged(*Hz);
        } catch (...) {
        }
    }

    void onAppFocus(std::string_view text)
    {
        try {
            if (const auto [enable] = std::get<1>(ee::tci::deserialize<bool>(text)); enable)
                emit m_context.appFocusChanged(*enable);
        } catch (...) {
        }
    }

    void onRxSensors(std::string_view text)
    {
        try {
            if (const auto [transceiver, dBm] = std::get<1>(ee::tci::deserialize<uint32_t, double>(text)); transceiver && dBm)
                emit m_context.rxSensors(*transceiver, *dBm);
        } catch (...) {
        }
    }

    void onTxSensors(std::string_view text)
    {
        try {
            if (const auto [transceiver, mic, rms, peak, swr] = std::get<1>(ee::tci::deserialize<uint32_t, double, double, double, double>(text)); transceiver && mic && rms && peak && swr)
                emit m_context.txSensors(*transceiver, *mic, *rms, *peak, *swr);
        } catch (...) {
        }
    }

    void onConnectStatus(TciClient::Status st)
    {
        if (m_status != st) {
            m_status = st;
            emit m_context.statusChanged(m_status);
        }
    }

private:
    TciClient& m_context;
    QWebSocket m_socket;
    QString m_address { "ws://localhost:50001" };
    TciClient::Status m_status { TciClient::Status::Disconnected };
    std::map<ee::tci::Command, std::function<void(std::string_view)>> m_executors;

private:
    struct state_t {
        state_t() { }
        virtual ~state_t() { }
        virtual void onEnter() { }
        virtual void onExit() { }
        virtual void connect(const QString&) { }
        virtual void disconnect() { }
        virtual void onConnected() { }
        virtual void onDisconnected() { }
    };

    struct idle_t : state_t {
        idle_t(impl& context)
            : m_context { context }
        {
        }

        void connect(const QString& address) override
        {
            m_context.m_address = address;
            m_context.switchState(m_context.m_connecting);
        }

    private:
        impl& m_context;
    };

    struct connecting_t : state_t {
        connecting_t(impl& context)
            : m_context { context }
        {
        }

        void onEnter() override
        {
            m_context.onConnectStatus(TciClient::Status::Connecting);
            m_context.m_socket.open(QUrl { m_context.m_address });
        }

        void disconnect() override
        {
            m_context.switchState(m_context.m_idle);
            m_context.m_socket.close();
            m_context.onConnectStatus(TciClient::Status::Disconnected);
        }

        void onConnected() override
        {
            m_context.switchState(m_context.m_connected);
        }

        void onDisconnected() override
        {
            onEnter(); // повторяем процедуру подключения
        }

    private:
        impl& m_context;
    };

    struct connected_t : state_t {
        connected_t(impl& context)
            : m_context { context }
        {
        }

        void onEnter() override
        {
            m_context.onConnectStatus(TciClient::Status::Connected);
        }

        void disconnect() override
        {
            m_context.switchState(m_context.m_idle);
            m_context.m_socket.close();
            m_context.onConnectStatus(TciClient::Status::Disconnected);
        }

        void onDisconnected() override
        {
            m_context.switchState(m_context.m_connecting);
        }

    private:
        impl& m_context;
    };

    void switchState(state_t& state)
    {
        if (pCurrentState != &state) {
            pCurrentState->onExit();
            pCurrentState = &state;
            pCurrentState->onEnter();
        }
    }

private:
    idle_t m_idle { *this };
    connecting_t m_connecting { *this };
    connected_t m_connected { *this };
    state_t* pCurrentState { &m_idle };
};

TciClient::TciClient(QObject* parent)
    : QObject { parent }
    , pImpl { std::make_unique<impl>(*this) }
{
}

TciClient::~TciClient()
{
}

TciClient::Status TciClient::status() const noexcept
{
    return pImpl->status();
}

void TciClient::connect(const QString& address)
{
    pImpl->connect(address);
}

void TciClient::disconnect()
{
    pImpl->disconnect();
}

void TciClient::start()
{
    pImpl->start();
}

void TciClient::stop()
{
    pImpl->stop();
}

void TciClient::setDds(quint32 transceiver, double Hz)
{
    pImpl->setDds(transceiver, Hz);
}

void TciClient::setIf(quint32 transceiver, quint32 channel, double Hz)
{
    pImpl->setIf(transceiver, channel, Hz);
}

void TciClient::setVfo(quint32 transceiver, quint32 channel, double Hz)
{
    pImpl->setVfo(transceiver, channel, Hz);
}

void TciClient::setModulation(quint32 transceiver, const QString& modulation)
{
    pImpl->setModulation(transceiver, modulation);
}

void TciClient::setTrx(quint32 transceiver, bool enable, const QString& opt)
{
    pImpl->setTrx(transceiver, enable, opt);
}

void TciClient::setTune(quint32 transceiver, bool enable)
{
    pImpl->setTune(transceiver, enable);
}

void TciClient::setDrive(quint32 transceiver, double value)
{
    pImpl->setDrive(transceiver, value);
}

void TciClient::setTuneDrive(quint32 transceiver, double value)
{
    pImpl->setTuneDrive(transceiver, value);
}

void TciClient::setRitEnable(quint32 transceiver, bool enable)
{
    pImpl->setRitEnable(transceiver, enable);
}

void TciClient::setXitEnable(quint32 transceiver, bool enable)
{
    pImpl->setXitEnable(transceiver, enable);
}

void TciClient::setSplitEnable(quint32 transceiver, bool enable)
{
    pImpl->setSplitEnable(transceiver, enable);
}

void TciClient::setRitOffset(quint32 transceiver, double Hz)
{
    pImpl->setRitOffset(transceiver, Hz);
}

void TciClient::setXitOffset(quint32 transceiver, double Hz)
{
    pImpl->setXitOffset(transceiver, Hz);
}

void TciClient::setTransceiverEnable(quint32 transceiver, bool enable)
{
    pImpl->setTransceiverEnable(transceiver, enable);
}

void TciClient::setRxChannelEnable(quint32 transceiver, quint32 channel, bool enable)
{
    pImpl->setRxChannelEnable(transceiver, channel, enable);
}

void TciClient::setRxFilterBand(quint32 transceiver, double low, double high)
{
    pImpl->setRxFilterBand(transceiver, low, high);
}

void TciClient::setCwMacrosSpeed(quint32 wpm)
{
    pImpl->setCwMacrosSpeed(wpm);
}

void TciClient::setCwMacrosDelay(quint32 ms)
{
    pImpl->setCwMacrosDelay(ms);
}

void TciClient::setCwMacrosSpeedUp(quint32 wpm)
{
    pImpl->setCwMacrosSpeedUp(wpm);
}

void TciClient::setCwMacrosSpeedDown(quint32 wpm)
{
    pImpl->setCwMacrosSpeedDown(wpm);
}

void TciClient::setCwKeyerSpeed(quint32 wpm)
{
    pImpl->setCwKeyerSpeed(wpm);
}

void TciClient::setVolume(double dB)
{
    pImpl->setVolume(dB);
}

void TciClient::setRxVolume(quint32 transceiver, quint32 channel, double dB)
{
    pImpl->setRxVolume(transceiver, channel, dB);
}

void TciClient::setRxBalance(quint32 transceiver, quint32 channel, double dB)
{
    pImpl->setRxBalance(transceiver, channel, dB);
}

void TciClient::setMute(bool enable)
{
    pImpl->setMute(enable);
}

void TciClient::setRxMute(quint32 transceiver, bool enable)
{
    pImpl->setRxMute(transceiver, enable);
}

void TciClient::setMonEnable(bool enable)
{
    pImpl->setMonEnable(enable);
}

void TciClient::setMonVolume(double dB)
{
    pImpl->setMonVolume(dB);
}

void TciClient::setAgcMode(quint32 transceiver, TciClient::AgcMode mode)
{
    pImpl->setAgcMode(transceiver, mode);
}

void TciClient::setAgcGain(quint32 transceiver, double dB)
{
    pImpl->setAgcGain(transceiver, dB);
}

void TciClient::setNbEnable(quint32 transceiver, bool enable)
{
    pImpl->setNbEnable(transceiver, enable);
}

void TciClient::setNbParam(quint32 transceiver, double threshold, double time)
{
    pImpl->setNbParam(transceiver, threshold, time);
}

void TciClient::setBinauralEnable(quint32 transceiver, bool enable)
{
    pImpl->setBinauralEnable(transceiver, enable);
}

void TciClient::setNrEnable(quint32 transceiver, bool enable)
{
    pImpl->setNrEnable(transceiver, enable);
}

void TciClient::setAncEnable(quint32 transceiver, bool enable)
{
    pImpl->setAncEnable(transceiver, enable);
}

void TciClient::setApfEnable(quint32 transceiver, bool enable)
{
    pImpl->setApfEnable(transceiver, enable);
}

void TciClient::setDseEnable(quint32 transceiver, bool enable)
{
    pImpl->setDseEnable(transceiver, enable);
}

void TciClient::setNfEnable(quint32 transceiver, bool enable)
{
    pImpl->setNfEnable(transceiver, enable);
}

void TciClient::setLock(quint32 transceiver, bool enable)
{
    pImpl->setLock(transceiver, enable);
}

void TciClient::setSquelchEnable(quint32 transceiver, bool enable)
{
    pImpl->setSquelchEnable(transceiver, enable);
}

void TciClient::setSquelchThreshold(quint32 transceiver, double dB)
{
    pImpl->setSquelchThreshold(transceiver, dB);
}

void TciClient::setDiglOffset(double Hz)
{
    pImpl->setDiglOffset(Hz);
}

void TciClient::setDiguOffset(double Hz)
{
    pImpl->setDiguOffset(Hz);
}

void TciClient::setSpot(const QString& callsign, const QString& modulation, double Hz, uint32_t argb, const QString& text)
{
    pImpl->setSpot(callsign, modulation, Hz, argb, text);
}

void TciClient::deleteSpot(const QString& callsign)
{
    pImpl->deleteSpot(callsign);
}

void TciClient::clearSpot()
{
    pImpl->clearSpot();
}

void TciClient::setAudioSamplesPerFrame(quint32 samples)
{
    pImpl->setAudioSamplesPerFrame(samples);
}

void TciClient::iqStart(quint32 transceiver)
{
    pImpl->iqStart(transceiver);
}

void TciClient::iqStop(quint32 transceiver)
{
    pImpl->iqStop(transceiver);
}

void TciClient::audioStart(quint32 transceiver)
{
    pImpl->audioStart(transceiver);
}

void TciClient::audioStop(quint32 transceiver)
{
    pImpl->audioStop(transceiver);
}

void TciClient::lineoutStart(quint32 transceiver)
{
    pImpl->lineoutStart(transceiver);
}

void TciClient::lineoutStop(quint32 transceiver)
{
    pImpl->lineoutStop(transceiver);
}

void TciClient::lineoutRecordStart(quint32 transceiver, quint32 sec)
{
    pImpl->lineoutRecordStart(transceiver, sec);
}

void TciClient::lineoutRecordSave(quint32 transceiver, const QString& fileName)
{
    pImpl->lineoutRecordSave(transceiver, fileName);
}

void TciClient::lineoutRecordBreak(quint32 transceiver)
{
    pImpl->lineoutRecordBreak(transceiver);
}

void TciClient::setAppInFocus()
{
    pImpl->setAppInFocus();
}

void TciClient::setRxSensorsEnable(bool enable, quint32 ms)
{
    pImpl->setRxSensorsEnable(enable, ms);
}

void TciClient::setTxSensorsEnable(bool enable, quint32 ms)
{
    pImpl->setTxSensorsEnable(enable, ms);
}
