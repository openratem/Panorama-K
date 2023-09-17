#pragma once

#include <vector>
#include <optional>
#include <QString>
#include <QObject>
#include <QSerialPort>
#include <gsl/span>

#include "../RigAbstract.h"

#include "parser/RigParser.h"
#include "converter/cmd/RigCmd.h"

namespace PanoramaK {

class Rig final : public RigAbstract
{
    Q_OBJECT

public:
    explicit Rig(QSerialPort &serialPort, const QString &file, QObject *parent = nullptr);

public slots:
    void open(quint32 polling, quint32 timeout) override;

    void setVfo(quint32 channel, quint32 Hz) override;

    void setRitOffset(qint32 Hz) override;

    void setRit(bool enable) override;

    void setXit(bool enable) override;

    void setSplit(bool enable) override;

    void setTrx(bool state) override;

    void setMode(Mode value) override;

private:
    Q_DISABLE_COPY(Rig)

    void init();

    void write(std::string_view bytes);
    void write(const QByteArray &bytes);

    void process(gsl::span<RigValueState> list);
    void process(gsl::span<CmdAbstract::CmdId> list);

    void setConnected(bool status);

private:
    QSerialPort &m_port;
    RigParser    m_parser;

    quint32 m_polling {0};
    quint32 m_timeout {0};

    std::vector<RigCmds> m_initCommands;
    std::vector<RigCmds> m_statusCommands;
    std::vector<RigCmds> m_setCommands;
    std::vector<std::string> m_lastStatus;

private:
    struct state_t {
        state_t(){}
        virtual ~state_t(){}
        virtual void init() {}
        virtual void finit() {}
        virtual void open() {}
        virtual void checkParams() {}
        virtual void setAnswer(std::string_view) {}
        virtual void timeout() {}
    };

    // состояние ожидания
    struct idle_t : state_t {
        idle_t(Rig &context) : m_context{context} {}
        void init() override;
        void finit() override;
        void open() override;
    private:
        Rig &m_context;
    };

    // состояние подключения к устройству
    struct connect_t : state_t {
        connect_t(Rig &context) : m_context{context} {}
        void init() override;
        void finit() override;
        void setAnswer(std::string_view bytes) override;
        void timeout() override;
    private:
        Rig &m_context;
        std::string m_answer;
        std::size_t m_count {0};
        RigCmds *pCmd {nullptr};
    };

    // чтение параметров устройства
    struct status_t : state_t {
        status_t(Rig &context) : m_context{context} {}
        void init() override;
        void finit() override;
        void setAnswer(std::string_view bytes) override;
        void timeout() override;
        void checkParams() override;
    private:
        Rig &m_context;
        std::string m_answer;
        std::size_t m_count {0};
        RigCmds *pCmd {nullptr};
        bool m_updatedParams {false};
    };

    // установка параметров устройству
    struct set_params_t : state_t {
        set_params_t(Rig &context) : m_context{context} {}
        void init() override;
        void finit() override;
        void setAnswer(std::string_view bytes) override;
        void timeout() override;
    private:
        Rig &m_context;
        std::string m_answer;
        std::size_t m_count {0};
        RigCmds *pCmd {nullptr};
    };

private:
    void switchState(state_t &state) {
        if (pCurrentState != &state) {
            pCurrentState->finit();
            pCurrentState = &state;
            pCurrentState->init();
        }
    }

private:
    idle_t m_idleState {*this};
    connect_t m_connectState {*this};
    status_t m_statusState {*this};
    set_params_t m_setParamsState {*this};
    state_t *pCurrentState {&m_idleState};

    std::optional<quint32> m_vfoA {1910000};
    std::optional<quint32> m_vfoB;
    std::optional<quint32> m_ritOffset;
    std::optional<Mode> m_mode;
    std::optional<bool> m_rit;
    std::optional<bool> m_xit;
    std::optional<bool> m_trx;
    std::optional<bool> m_split;

    bool m_connected {false};

    QTimer m_timer;
    QTimer m_timerLoop;
    QEventLoop m_loop;
};

}  // namespace PanoramaK

