#include "Rig.h"
#include <QMap>

using namespace std::chrono_literals;

inline void wait(const std::chrono::milliseconds &ms)
{
    QEventLoop t_loop;
    QTimer     t_timer;

    QObject::connect(&t_timer, &QTimer::timeout, &t_loop, &QEventLoop::quit);
    t_timer.start(static_cast<int>(qMax<long>(1, ms.count())));
    t_loop.exec();
}

inline bool isSpace(char c) {
    return (c == '\n' || c == '\r');
}

static const QMap<QString, CmdAbstract::CmdId> MapGroup = {{"pmfreqa", CmdAbstract::pmFreqA},
                                                          {"pmfreqb", CmdAbstract::pmFreqB},
                                                          {"pmfreq" , CmdAbstract::pmFreq},
                                                          {"pmritoffset", CmdAbstract::pmRitOffset},
                                                          {"pmrit0", CmdAbstract::pmRit0},
                                                          {"pmpitch", CmdAbstract::pmPitch},
                                                          {"pmspliton", CmdAbstract::pmSplitOn},
                                                          {"pmsplitoff", CmdAbstract::pmSplitOff},
                                                          {"pmvfoa", CmdAbstract::pmVfoA},
                                                          {"pmvfob", CmdAbstract::pmVfoB},
                                                          {"pmvfoequal", CmdAbstract::pmVfoEqual},
                                                          {"pmvfoswap", CmdAbstract::pmVfoSwap},
                                                          {"pmvfoaa", CmdAbstract::pmVfoAA},
                                                          {"pmvfoab", CmdAbstract::pmVfoAB},
                                                          {"pmvfoba", CmdAbstract::pmVfoBA},
                                                          {"pmvfobb", CmdAbstract::pmVfoBB},
                                                          {"pmriton", CmdAbstract::pmRitOn},
                                                          {"pmritoff", CmdAbstract::pmRitOff},
                                                          {"pmxiton", CmdAbstract::pmXitOn},
                                                          {"pmxitoff", CmdAbstract::pmXitOff},
                                                          {"pmrx", CmdAbstract::pmRx},
                                                          {"pmtx", CmdAbstract::pmTx},
                                                          {"pmcw_u", CmdAbstract::pmCW_U},
                                                          {"pmcw_l", CmdAbstract::pmCW_L},
                                                          {"pmssb_u", CmdAbstract::pmSSB_U},
                                                          {"pmssb_l", CmdAbstract::pmSSB_L},
                                                          {"pmdig_u", CmdAbstract::pmDIG_U},
                                                          {"pmdig_l", CmdAbstract::pmDIG_L},
                                                          {"pmam", CmdAbstract::pmAM},
                                                          {"pmfm", CmdAbstract::pmFM}};

namespace PanoramaK {

Rig::Rig(QSerialPort &serialPort, const QString &file, QObject *parent) :
  RigAbstract{parent},
  m_port{serialPort},
  m_parser{file}

{
    init();

    pCurrentState->init();

    connect(&m_port, &QSerialPort::readyRead, this, [&](){
        const auto bytes = m_port.readAll();
        pCurrentState->setAnswer(std::string_view{bytes.data(), (std::size_t)bytes.size()});
    });
    connect(&m_timer, &QTimer::timeout, this, [&](){ pCurrentState->timeout(); });
    connect(&m_timerLoop, &QTimer::timeout, &m_loop, [&](){
        m_loop.quit();
        m_timerLoop.stop();
    });
}

void Rig::open(quint32 polling, quint32 timeout)
{
    m_polling = polling;
    m_timeout = timeout;

    pCurrentState->open();
}

void Rig::setVfo(quint32 channel, quint32 Hz)
{
    if (channel == 0) {
        m_vfoA = Hz;
        pCurrentState->checkParams();
    }
    else if (channel == 1) {
        m_vfoB = Hz;
        pCurrentState->checkParams();
    }
}

void Rig::setRitOffset(qint32 Hz)
{
    m_ritOffset = Hz;
    pCurrentState->checkParams();
}

void Rig::setRit(bool enable)
{
    m_rit = enable;
    pCurrentState->checkParams();
}

void Rig::setXit(bool enable)
{
    m_xit = enable;
    pCurrentState->checkParams();
}

void Rig::setSplit(bool enable)
{
    m_split = enable;
    pCurrentState->checkParams();
}

void Rig::setTrx(bool state)
{
    m_trx = state;
    pCurrentState->checkParams();
}

void Rig::setMode(Mode value)
{
    m_mode = value;
    pCurrentState->checkParams();
}

void Rig::init()
{
    m_setCommands.resize(CmdAbstract::XmdICount);

    for (auto &group : m_parser.groups()) {
        if (group.contains("init", Qt::CaseInsensitive)) {
            RigCmds t_cmds;
            for (auto &key : m_parser.keys(group)) {
                if (key.contains("command", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::Command, m_parser.value(group, key).toLatin1());
                else if (key.contains("replylength", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::ReplyLength, m_parser.value(group, key).toLatin1());
                else if (key.contains("replyend", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::ReplyEnd, m_parser.value(group, key).toLatin1());
                else if (key.contains("validate", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::Validate, m_parser.value(group, key).toLatin1());

            }
            m_initCommands.emplace_back(t_cmds);
        }
        else if (group.contains("status", Qt::CaseInsensitive)) {
            RigCmds t_cmds;
            for (auto &key : m_parser.keys(group)) {
                if (key.contains("command", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::Command, m_parser.value(group, key).toLatin1());
                else if (key.contains("replylength", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::ReplyLength, m_parser.value(group, key).toLatin1());
                else if (key.contains("replyend", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::ReplyEnd, m_parser.value(group, key).toLatin1());
                else if (key.contains("validate", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::Validate, m_parser.value(group, key).toLatin1());
                else if (key.contains("value", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::Value, m_parser.value(group, key).toLatin1());
                else if (key.contains("flag", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::Flag, m_parser.value(group, key).toLatin1());

            }
            m_statusCommands.emplace_back(t_cmds);
            m_lastStatus.emplace_back(std::string{});
        }
        else {
            RigCmds t_cmds;
            for (auto &key : m_parser.keys(group)) {
                if (key.contains("command", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::Command, m_parser.value(group, key).toLatin1());
                else if (key.contains("replylength", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::ReplyLength, m_parser.value(group, key).toLatin1());
                else if (key.contains("replyend", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::ReplyEnd, m_parser.value(group, key).toLatin1());
                else if (key.contains("validate", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::Validate, m_parser.value(group, key).toLatin1());
                else if (key.contains("value", Qt::CaseInsensitive))
                    t_cmds.initParam(RigCmds::Value, m_parser.value(group, key).toLatin1());
            }

            if (MapGroup.contains(group.toLower())) {
                CmdAbstract::CmdId t_id = MapGroup[group.toLower()];
                if (t_id > CmdAbstract::pmNone) {
                    if ((t_id == CmdAbstract::pmFreq) || (t_id == CmdAbstract::pmFreqA) || (t_id == CmdAbstract::pmFreqB) || (t_id == CmdAbstract::pmRitOffset))
                        t_cmds.setWorkMode(RigCmds::SetMode);

                    t_cmds.setCmdId(t_id);
                    m_setCommands.at(t_id) = t_cmds;
                }
            }

        }
    }
}

void Rig::write(std::string_view bytes)
{
    m_port.write(bytes.data(), bytes.size());
}

void Rig::write(const QByteArray &bytes)
{
    m_port.write(bytes);
}

void Rig::process(gsl::span<RigValueState> list)
{
    for (auto &name : list) {
        if ((name.cmd() == CmdAbstract::pmFreq) || (name.cmd() == CmdAbstract::pmFreqA)) {
            emit vfoChanged(0u, static_cast<quint32>(name.value()));
        }
        else if (name.cmd() == CmdAbstract::pmFreqB) {
            emit vfoChanged(1u, static_cast<quint32>(name.value()));
        }
        else if (name.cmd() == CmdAbstract::pmRitOffset) {
            emit ritOffsetChanged(name.value());
        }
    }
}

void Rig::process(gsl::span<CmdAbstract::CmdId> list)
{
    for (auto &cmdId : list) {
        switch (cmdId) {
        case CmdAbstract::pmSplitOn:
            emit splitChanged(true);
            break;

        case CmdAbstract::pmSplitOff:
            emit splitChanged(false);
            break;

        case CmdAbstract::pmRitOn:
            emit ritChanged(true);
            break;

        case CmdAbstract::pmRitOff:
            emit ritChanged(false);
            break;

        case CmdAbstract::pmXitOn:
            emit xitChanged(true);
            break;

        case CmdAbstract::pmXitOff:
            emit xitChanged(false);
            break;

        case CmdAbstract::pmRx:
            emit trxChanged(false);
            break;

        case CmdAbstract::pmTx:
            emit trxChanged(true);
            break;

        case CmdAbstract::pmCW_U:
            emit modeChanged(RigAbstract::CWU);
            break;

        case CmdAbstract::pmCW_L:
            emit modeChanged(RigAbstract::CWL);
            break;

        case CmdAbstract::pmSSB_U:
            emit modeChanged(RigAbstract::USB);
            break;

        case CmdAbstract::pmSSB_L:
            emit modeChanged(RigAbstract::LSB);
            break;

        case CmdAbstract::pmDIG_L:
            emit modeChanged(RigAbstract::DIGL);
            break;

        case CmdAbstract::pmDIG_U:
            emit modeChanged(RigAbstract::DIGU);
            break;

        case CmdAbstract::pmAM:
            emit modeChanged(RigAbstract::AM);
            break;

        case CmdAbstract::pmFM:
            emit modeChanged(RigAbstract::FM);
            break;

        default:
            break;
        }
    }
}

void Rig::setConnected(bool status)
{
    if (m_connected != status) {
        m_connected = status;
        if (m_connected)
            emit connected();
        else
            emit disconnected();
    }
}

//======================================================================================================================
// Состояние ожидания
void Rig::idle_t::init()
{
    m_context.setConnected(false);
}

void Rig::idle_t::finit()
{

}

void Rig::idle_t::open()
{
    m_context.switchState(m_context.m_connectState);
}

//======================================================================================================================
// Состояние подключения
void Rig::connect_t::init()
{
    m_context.setConnected(false);

    m_answer.clear();
    pCmd    = nullptr;
    m_count = 0;
    // отправляем команды инициализации
    for (auto &cmd : m_context.m_initCommands) {
        // записываем команду в COM порт
        m_context.write(cmd.command());
        // если требуется дождаться ответа
        if (cmd.waitAnswer()) {
            // запоминаем текущую команду
            pCmd = &cmd;
        }
        else {
            // ожидаем время опроса
            wait(std::chrono::milliseconds{m_context.m_polling});
        }
    }

    // переходим в состояние установки параметров
    m_context.switchState(m_context.m_setParamsState);
}

void Rig::connect_t::finit()
{

}

void Rig::connect_t::setAnswer(std::string_view bytes)
{

}

void Rig::connect_t::timeout()
{

}

//======================================================================================================================
// Состояние чтения параметров устройства
void Rig::status_t::init()
{
    m_context.setConnected(true);

    m_answer.clear();
    pCmd    = nullptr;
    m_count = 0;
    m_updatedParams = false;

    m_context.m_timer.stop();
    m_context.m_timer.start(m_context.m_polling);
}

void Rig::status_t::finit()
{
    for (auto &str : m_context.m_lastStatus)
        str.clear();
}

void Rig::status_t::setAnswer(std::string_view bytes)
{
    // добавляем принятые байты в конец
    m_answer += bytes;

    // проверка корректности ответа
    if (pCmd) {
        if (pCmd->textFormat())
            m_answer.erase(std::remove_if(m_answer.begin(), m_answer.end(), isSpace), m_answer.end());

        if (pCmd->validAnswer(QByteArray::fromStdString(m_answer))) {
            m_context.m_timerLoop.stop();
            m_context.m_loop.quit();
        }
    }
}

void Rig::status_t::timeout()
{
    // останавливаем таймер
    m_context.m_timer.stop();
    // отправляем команды инициализации
    std::size_t t_index {0};
    for (auto &cmd : m_context.m_statusCommands) {
repeat_status:
        // записываем команду в COM порт
        m_context.write(cmd.command());
        // если требуется дождаться ответа
        if (cmd.waitAnswer()) {
            // запоминаем текущую команду
            pCmd = &cmd;
            // ожидаем ответа
            m_context.m_timerLoop.start(m_context.m_timeout);
            m_context.m_loop.exec();

            // если был изменён хотя бы один параметр переключаемся в установку параметров
            if (m_updatedParams)
                goto set_params;

            // если ответ правильный, тогда обрабатываем сообщение
            if (cmd.validAnswer(QByteArray::fromStdString(m_answer))) {
                // передаём ответ на обработку
                cmd.setAnswer(QByteArray::fromStdString(m_answer));
                // обработка ответа
                m_context.process(cmd.values());
                m_context.process(cmd.states());

                //TODO если ответ отличается от предыдущего, тогда повторяем опрос этого же параметра
                if (m_context.m_lastStatus.at(t_index).empty()) {
                    m_context.m_lastStatus.at(t_index) = m_answer;
                }
                else {
                    if (m_answer.compare(m_context.m_lastStatus.at(t_index)) != 0) {
                        m_context.m_lastStatus.at(t_index) = m_answer;
                        m_answer.clear();
                        // повторяем опрос
                        goto repeat_status;
                    }
                }
            }
            else {
                // ответ неправильный, прекращаем опрос и переключаемся в состояние подключения
                m_context.switchState(m_context.m_connectState);
                return;
            }
        }

        // если был изменён хотя бы один параметр переключаемся в установку параметров
        if (m_updatedParams)
            goto set_params;

        // очистка ответа
        m_answer.clear();
        pCmd = nullptr;

        // если был изменён хотя бы один параметр переключаемся в установку параметров
        if (m_updatedParams)
            goto set_params;

        //
        ++t_index;
    }

    // повторяем опрос
    init();
    return;

set_params:
    // переходим в состояние установки параметров
    m_context.switchState(m_context.m_setParamsState);
}

void Rig::status_t::checkParams()
{
    m_updatedParams = true;
    m_context.m_loop.quit();
}

//======================================================================================================================
// Состояние установки параметров устройству
void Rig::set_params_t::init()
{
    m_context.setConnected(true);

    m_answer.clear();
    m_count = 0;
    pCmd = nullptr;

    m_context.m_timer.stop();
    m_context.m_timer.start(m_context.m_polling);
}

void Rig::set_params_t::finit()
{

}

void Rig::set_params_t::setAnswer(std::string_view bytes)
{
    // добавляем принятые байты в конец
    m_answer += bytes;

    // проверка корректности ответа
    if (pCmd) {
        if (pCmd->textFormat())
            m_answer.erase(std::remove_if(m_answer.begin(), m_answer.end(), isSpace), m_answer.end());

        if (pCmd->validAnswer(QByteArray::fromStdString(m_answer))) {
            m_context.m_timerLoop.stop();
            m_context.m_loop.quit();
        }
    }
}

void Rig::set_params_t::timeout()
{
retry:
    bool t_writed {false};

    // останавливаем таймер
    m_context.m_timer.stop();

    // запись всех параметров в устройство
    if (m_context.m_vfoA.has_value()) {
        m_answer.clear();
        t_writed = true;
        if (m_context.m_setCommands[CmdAbstract::pmFreqA].cmdId() != CmdAbstract::pmNone) {
            // установка частоты VFOA
            m_context.write(m_context.m_setCommands[CmdAbstract::pmFreqA].command(*m_context.m_vfoA));
            // если требуется дождаться ответа
            if (m_context.m_setCommands[CmdAbstract::pmFreqA].waitAnswer()) {
                // запоминаем текущую команду
                pCmd = &m_context.m_setCommands[CmdAbstract::pmFreqA];
                // ожидаем ответа
                m_context.m_timerLoop.start(m_context.m_timeout);
                m_context.m_loop.exec();
                // TODO прописать валидацию ответа
                m_answer.clear();
            }
            else {
                // очищаем
                m_context.m_vfoA.reset();
                // ожидаем время опроса
                wait(std::chrono::milliseconds{m_context.m_polling});
            }
        }
        else if (m_context.m_setCommands[CmdAbstract::pmFreq].cmdId() != CmdAbstract::pmNone) {
            m_context.write(m_context.m_setCommands[CmdAbstract::pmFreq].command(*m_context.m_vfoA));
            // если требуется дождаться ответа
            if (m_context.m_setCommands[CmdAbstract::pmFreq].waitAnswer()) {
                // запоминаем текущую команду
                pCmd = &m_context.m_setCommands[CmdAbstract::pmFreq];
                // ожидаем ответа
                m_context.m_timerLoop.start(m_context.m_timeout);
                m_context.m_loop.exec();
                // TODO прописать валидацию ответа
                m_answer.clear();
            }
            else {
                // ожидаем время опроса
                wait(std::chrono::milliseconds{m_context.m_polling});
            }
        }
    }

    if (m_context.m_vfoB.has_value()) {
        m_answer.clear();
        t_writed = true;
        if (m_context.m_setCommands[CmdAbstract::pmFreqB].cmdId() != CmdAbstract::pmNone) {
            // установка частоты VFOB
            m_context.write(m_context.m_setCommands[CmdAbstract::pmFreqB].command(*m_context.m_vfoB));
            // если требуется дождаться ответа
            if (m_context.m_setCommands[CmdAbstract::pmFreqB].waitAnswer()) {
                // запоминаем текущую команду
                pCmd = &m_context.m_setCommands[CmdAbstract::pmFreqB];
                // ожидаем ответа
                m_context.m_timerLoop.start(m_context.m_timeout);
                m_context.m_loop.exec();
                // TODO прописать валидацию ответа
                m_answer.clear();
            }
            else {
                // очищаем
                m_context.m_vfoB.reset();
                // ожидаем время опроса
                wait(std::chrono::milliseconds{m_context.m_polling});
            }
        }
    }

    if (m_context.m_mode.has_value()) {
        m_answer.clear();
        t_writed = true;
        switch (*m_context.m_mode) {
            case RigAbstract::AM:
                if (m_context.m_setCommands[CmdAbstract::pmAM].cmdId() != CmdAbstract::pmNone) {
                    m_context.write(m_context.m_setCommands[CmdAbstract::pmAM].command());
                    // если требуется дождаться ответа
                    if (m_context.m_setCommands[CmdAbstract::pmAM].waitAnswer()) {
                        // запоминаем текущую команду
                        pCmd = &m_context.m_setCommands[CmdAbstract::pmAM];
                        // ожидаем ответа
                        m_context.m_timerLoop.start(m_context.m_timeout);
                        m_context.m_loop.exec();
                        // TODO прописать валидацию ответа
                        m_answer.clear();
                    }
                    else {
                        // очищаем
                        m_context.m_mode.reset();
                        // ожидаем время опроса
                        wait(std::chrono::milliseconds{m_context.m_polling});
                    }
                }
                break;

            case RigAbstract::FM:
                if (m_context.m_setCommands[CmdAbstract::pmFM].cmdId() != CmdAbstract::pmNone) {
                    m_context.write(m_context.m_setCommands[CmdAbstract::pmFM].command());
                    // если требуется дождаться ответа
                    if (m_context.m_setCommands[CmdAbstract::pmFM].waitAnswer()) {
                        // запоминаем текущую команду
                        pCmd = &m_context.m_setCommands[CmdAbstract::pmFM];
                        // ожидаем ответа
                        m_context.m_timerLoop.start(m_context.m_timeout);
                        m_context.m_loop.exec();
                        // TODO прописать валидацию ответа
                        m_answer.clear();
                    }
                    else {
                        // очищаем
                        m_context.m_mode.reset();
                        // ожидаем время опроса
                        wait(std::chrono::milliseconds{m_context.m_polling});
                    }
                }
                break;

            case RigAbstract::CWL:
                if (m_context.m_setCommands[CmdAbstract::pmCW_L].cmdId() != CmdAbstract::pmNone) {
                    m_context.write(m_context.m_setCommands[CmdAbstract::pmCW_L].command());
                    // если требуется дождаться ответа
                    if (m_context.m_setCommands[CmdAbstract::pmCW_L].waitAnswer()) {
                        // запоминаем текущую команду
                        pCmd = &m_context.m_setCommands[CmdAbstract::pmCW_L];
                        // ожидаем ответа
                        m_context.m_timerLoop.start(m_context.m_timeout);
                        m_context.m_loop.exec();
                        // TODO прописать валидацию ответа
                        m_answer.clear();
                    }
                    else {
                        // очищаем
                        m_context.m_mode.reset();
                        // ожидаем время опроса
                        wait(std::chrono::milliseconds{m_context.m_polling});
                    }
                }
                break;

            case RigAbstract::CWU:
                if (m_context.m_setCommands[CmdAbstract::pmCW_U].cmdId() != CmdAbstract::pmNone) {
                    m_context.write(m_context.m_setCommands[CmdAbstract::pmCW_U].command());
                    // если требуется дождаться ответа
                    if (m_context.m_setCommands[CmdAbstract::pmCW_U].waitAnswer()) {
                        // запоминаем текущую команду
                        pCmd = &m_context.m_setCommands[CmdAbstract::pmCW_U];
                        // ожидаем ответа
                        m_context.m_timerLoop.start(m_context.m_timeout);
                        m_context.m_loop.exec();
                        // TODO прописать валидацию ответа
                        m_answer.clear();
                    }
                    else {
                        // очищаем
                        m_context.m_mode.reset();
                        // ожидаем время опроса
                        wait(std::chrono::milliseconds{m_context.m_polling});
                    }
                }
                break;

            case RigAbstract::USB:
                if (m_context.m_setCommands[CmdAbstract::pmSSB_U].cmdId() != CmdAbstract::pmNone) {
                    m_context.write(m_context.m_setCommands[CmdAbstract::pmSSB_U].command());
                    // если требуется дождаться ответа
                    if (m_context.m_setCommands[CmdAbstract::pmSSB_U].waitAnswer()) {
                        // запоминаем текущую команду
                        pCmd = &m_context.m_setCommands[CmdAbstract::pmSSB_U];
                        // ожидаем ответа
                        m_context.m_timerLoop.start(m_context.m_timeout);
                        m_context.m_loop.exec();
                        // TODO прописать валидацию ответа
                        m_answer.clear();
                    }
                    else {
                        // очищаем
                        m_context.m_mode.reset();
                        // ожидаем время опроса
                        wait(std::chrono::milliseconds{m_context.m_polling});
                    }
                }
                break;

            case RigAbstract::LSB:
                if (m_context.m_setCommands[CmdAbstract::pmSSB_L].cmdId() != CmdAbstract::pmNone) {
                    m_context.write(m_context.m_setCommands[CmdAbstract::pmSSB_L].command());
                    // если требуется дождаться ответа
                    if (m_context.m_setCommands[CmdAbstract::pmSSB_L].waitAnswer()) {
                        // запоминаем текущую команду
                        pCmd = &m_context.m_setCommands[CmdAbstract::pmSSB_L];
                        // ожидаем ответа
                        m_context.m_timerLoop.start(m_context.m_timeout);
                        m_context.m_loop.exec();
                        // TODO прописать валидацию ответа
                        m_answer.clear();
                    }
                    else {
                        // очищаем
                        m_context.m_mode.reset();
                        // ожидаем время опроса
                        wait(std::chrono::milliseconds{m_context.m_polling});
                    }
                }
                break;

            case RigAbstract::DIGL:
                if (m_context.m_setCommands[CmdAbstract::pmDIG_L].cmdId() != CmdAbstract::pmNone) {
                    m_context.write(m_context.m_setCommands[CmdAbstract::pmDIG_L].command());
                    // если требуется дождаться ответа
                    if (m_context.m_setCommands[CmdAbstract::pmDIG_L].waitAnswer()) {
                        // запоминаем текущую команду
                        pCmd = &m_context.m_setCommands[CmdAbstract::pmDIG_L];
                        // ожидаем ответа
                        m_context.m_timerLoop.start(m_context.m_timeout);
                        m_context.m_loop.exec();
                        // TODO прописать валидацию ответа
                        m_answer.clear();
                    }
                    else {
                        // очищаем
                        m_context.m_mode.reset();
                        // ожидаем время опроса
                        wait(std::chrono::milliseconds{m_context.m_polling});
                    }
                }
                break;

            case RigAbstract::DIGU:
                if (m_context.m_setCommands[CmdAbstract::pmDIG_U].cmdId() != CmdAbstract::pmNone) {
                    m_context.write(m_context.m_setCommands[CmdAbstract::pmDIG_U].command());
                    // если требуется дождаться ответа
                    if (m_context.m_setCommands[CmdAbstract::pmDIG_U].waitAnswer()) {
                        // запоминаем текущую команду
                        pCmd = &m_context.m_setCommands[CmdAbstract::pmDIG_U];
                        // ожидаем ответа
                        m_context.m_timerLoop.start(m_context.m_timeout);
                        m_context.m_loop.exec();
                        // TODO прописать валидацию ответа
                        m_answer.clear();
                    }
                    else {
                        // очищаем
                        m_context.m_mode.reset();
                        // ожидаем время опроса
                        wait(std::chrono::milliseconds{m_context.m_polling});
                    }
                }
                break;

            default:
                break;
        }
    }

    if (m_context.m_rit.has_value()) {
        m_answer.clear();
        t_writed = true;
        if (*m_context.m_rit) {
            if (m_context.m_setCommands[CmdAbstract::pmRitOn].cmdId() != CmdAbstract::pmNone) {
                // установка частоты VFOB
                m_context.write(m_context.m_setCommands[CmdAbstract::pmRitOn].command(*m_context.m_rit));
                // если требуется дождаться ответа
                if (m_context.m_setCommands[CmdAbstract::pmRitOn].waitAnswer()) {
                    // запоминаем текущую команду
                    pCmd = &m_context.m_setCommands[CmdAbstract::pmRitOn];
                    // ожидаем ответа
                    m_context.m_timerLoop.start(m_context.m_timeout);
                    m_context.m_loop.exec();
                    // TODO прописать валидацию ответа
                    m_answer.clear();
                }
                else {
                    // очищаем
                    m_context.m_rit.reset();
                    // ожидаем время опроса
                    wait(std::chrono::milliseconds{m_context.m_polling});
                }
            }
        }
        else {
            if (m_context.m_setCommands[CmdAbstract::pmRitOff].cmdId() != CmdAbstract::pmNone) {
                // установка частоты VFOB
                m_context.write(m_context.m_setCommands[CmdAbstract::pmRitOff].command(*m_context.m_rit));
                // если требуется дождаться ответа
                if (m_context.m_setCommands[CmdAbstract::pmRitOff].waitAnswer()) {
                    // запоминаем текущую команду
                    pCmd = &m_context.m_setCommands[CmdAbstract::pmRitOff];
                    // ожидаем ответа
                    m_context.m_timerLoop.start(m_context.m_timeout);
                    m_context.m_loop.exec();
                    // TODO прописать валидацию ответа
                    m_answer.clear();
                }
                else {
                    // очищаем
                    m_context.m_rit.reset();
                    // ожидаем время опроса
                    wait(std::chrono::milliseconds{m_context.m_polling});
                }
            }
        }
    }

    if (m_context.m_ritOffset.has_value()) {
        m_answer.clear();
        t_writed = true;
        if (m_context.m_setCommands[CmdAbstract::pmRitOffset].cmdId() != CmdAbstract::pmNone) {
            // установка частоты расстройки приёмника
            m_context.write(m_context.m_setCommands[CmdAbstract::pmRitOffset].command(*m_context.m_ritOffset));
            // если требуется дождаться ответа
            if (m_context.m_setCommands[CmdAbstract::pmRitOffset].waitAnswer()) {
                // запоминаем текущую команду
                pCmd = &m_context.m_setCommands[CmdAbstract::pmRitOffset];
                // ожидаем ответа
                m_context.m_timerLoop.start(m_context.m_timeout);
                m_context.m_loop.exec();
                // TODO прописать валидацию ответа
                m_answer.clear();
            }
            else {
                // очищаем
                m_context.m_ritOffset.reset();
                // ожидаем время опроса
                wait(std::chrono::milliseconds{m_context.m_polling});
            }
        }
    }

    if (m_context.m_xit.has_value()) {
        m_answer.clear();
        t_writed = true;
        if (*m_context.m_xit) {
            if (m_context.m_setCommands[CmdAbstract::pmXitOn].cmdId() != CmdAbstract::pmNone) {
                // установка частоты VFOB
                m_context.write(m_context.m_setCommands[CmdAbstract::pmXitOn].command(*m_context.m_xit));
                // если требуется дождаться ответа
                if (m_context.m_setCommands[CmdAbstract::pmXitOn].waitAnswer()) {
                    // запоминаем текущую команду
                    pCmd = &m_context.m_setCommands[CmdAbstract::pmXitOn];
                    // ожидаем ответа
                    m_context.m_timerLoop.start(m_context.m_timeout);
                    m_context.m_loop.exec();
                    // TODO прописать валидацию ответа
                    m_answer.clear();
                }
                else {
                    // очищаем
                    m_context.m_xit.reset();
                    // ожидаем время опроса
                    wait(std::chrono::milliseconds{m_context.m_polling});
                }
            }
        }
        else {
            if (m_context.m_setCommands[CmdAbstract::pmXitOff].cmdId() != CmdAbstract::pmNone) {
                // установка частоты VFOB
                m_context.write(m_context.m_setCommands[CmdAbstract::pmXitOff].command(*m_context.m_xit));
                // если требуется дождаться ответа
                if (m_context.m_setCommands[CmdAbstract::pmXitOff].waitAnswer()) {
                    // запоминаем текущую команду
                    pCmd = &m_context.m_setCommands[CmdAbstract::pmXitOff];
                    // ожидаем ответа
                    m_context.m_timerLoop.start(m_context.m_timeout);
                    m_context.m_loop.exec();
                    // TODO прописать валидацию ответа
                    m_answer.clear();
                }
                else {
                    // очищаем
                    m_context.m_xit.reset();
                    // ожидаем время опроса
                    wait(std::chrono::milliseconds{m_context.m_polling});
                }
            }
        }
    }

    if (m_context.m_split.has_value()) {
        m_answer.clear();
        t_writed = true;
        if (*m_context.m_split) {
            if (m_context.m_setCommands[CmdAbstract::pmSplitOn].cmdId() != CmdAbstract::pmNone) {
                // установка частоты VFOB
                m_context.write(m_context.m_setCommands[CmdAbstract::pmSplitOn].command(*m_context.m_split));
                // если требуется дождаться ответа
                if (m_context.m_setCommands[CmdAbstract::pmSplitOn].waitAnswer()) {
                    // запоминаем текущую команду
                    pCmd = &m_context.m_setCommands[CmdAbstract::pmSplitOn];
                    // ожидаем ответа
                    m_context.m_timerLoop.start(m_context.m_timeout);
                    m_context.m_loop.exec();
                    // TODO прописать валидацию ответа
                    m_answer.clear();
                }
                else {
                    // очищаем
                    m_context.m_split.reset();
                    // ожидаем время опроса
                    wait(std::chrono::milliseconds{m_context.m_polling});
                }
            }
        }
        else {
            if (m_context.m_setCommands[CmdAbstract::pmSplitOff].cmdId() != CmdAbstract::pmNone) {
                // установка частоты VFOB
                m_context.write(m_context.m_setCommands[CmdAbstract::pmSplitOff].command(*m_context.m_split));
                // если требуется дождаться ответа
                if (m_context.m_setCommands[CmdAbstract::pmSplitOff].waitAnswer()) {
                    // запоминаем текущую команду
                    pCmd = &m_context.m_setCommands[CmdAbstract::pmSplitOff];
                    // ожидаем ответа
                    m_context.m_timerLoop.start(m_context.m_timeout);
                    m_context.m_loop.exec();
                    // TODO прописать валидацию ответа
                    m_answer.clear();
                }
                else {
                    // очищаем
                    m_context.m_split.reset();
                    // ожидаем время опроса
                    wait(std::chrono::milliseconds{m_context.m_polling});
                }
            }
        }
    }

    if (m_context.m_trx.has_value()) {
        m_answer.clear();
        t_writed = true;
        if (*m_context.m_trx) {
            if (m_context.m_setCommands[CmdAbstract::pmTx].cmdId() != CmdAbstract::pmNone) {
                // установка частоты VFOB
                m_context.write(m_context.m_setCommands[CmdAbstract::pmTx].command(*m_context.m_trx));
                // если требуется дождаться ответа
                if (m_context.m_setCommands[CmdAbstract::pmTx].waitAnswer()) {
                    // запоминаем текущую команду
                    pCmd = &m_context.m_setCommands[CmdAbstract::pmTx];
                    // ожидаем ответа
                    m_context.m_timerLoop.start(m_context.m_timeout);
                    m_context.m_loop.exec();
                    // TODO прописать валидацию ответа
                    m_answer.clear();
                }
                else {
                    // очищаем
                    m_context.m_trx.reset();
                    // ожидаем время опроса
                    wait(std::chrono::milliseconds{m_context.m_polling});
                }
            }
        }
        else {
            if (m_context.m_setCommands[CmdAbstract::pmRx].cmdId() != CmdAbstract::pmNone) {
                // установка частоты VFOB
                m_context.write(m_context.m_setCommands[CmdAbstract::pmRx].command(*m_context.m_trx));
                // если требуется дождаться ответа
                if (m_context.m_setCommands[CmdAbstract::pmRx].waitAnswer()) {
                    // запоминаем текущую команду
                    pCmd = &m_context.m_setCommands[CmdAbstract::pmRx];
                    // ожидаем ответа
                    m_context.m_timerLoop.start(m_context.m_timeout);
                    m_context.m_loop.exec();
                    // TODO прописать валидацию ответа
                    m_answer.clear();
                }
                else {
                    // очищаем
                    m_context.m_trx.reset();
                    // ожидаем время опроса
                    wait(std::chrono::milliseconds{m_context.m_polling});
                }
            }
        }
    }

    // выполняем повторную установку параметров до тех пор пока есть неустановленные параметры
    if (t_writed)
        goto retry;

    // переключаемся в состояние опроса параметров радио устройства
    m_context.switchState(m_context.m_statusState);
}



}  // namespace PanoramaK




