#include "RigControl.h"

const QMap<QString, CmdAbstract::CmdId> RigControl::m_mapGroup = {{"pmfreqa", CmdAbstract::pmFreqA},
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

RigControl::RigControl(RigParser &parser, QObject *parent) :
  QThread(parent),
  m_open(false),
  m_timeout(0),
  m_poll(0),
  m_pause(false),
  m_parser(parser),
  pMainTimer(nullptr),
  m_vfoa(0),
  m_vfob(0),
  m_ritOffset(0),
  m_rit(false),
  m_xit(false),
  m_split(false),
  m_trx(false),
  m_mode(RigAbstract::AM)
{
    init();

    connect(&m_pauseTimer, &QTimer::timeout, this, &RigControl::onStartSurvey);
}

void RigControl::open(quint32 timeout, quint32 poll_ms)
{
    if (m_open)
        return;

    m_timeout = timeout;
    m_poll    = poll_ms;
    m_open    = true;
    start();
}
void RigControl::close()
{
    if (!m_open)
        return;

    m_open = false;
    quit();
    wait();
}
bool RigControl::isOpen() const
{
    return m_open;
}

PanoramaK::QueueEvent& RigControl::queue()
{
    return m_queue;
}

void RigControl::setVfo(quint32 channel, quint32 frequency)
{
    if (channel == 0u) {
        if (m_vfoa == frequency)
            return;

        pauseSurvey();
        m_vfoa = frequency;
        m_vfoaItem.setValue(m_vfoa);
        restoreSurvey();
    }
    else if (channel == 1u) {
        if (m_vfoa == frequency)
            return;

        pauseSurvey();
        m_vfob = frequency;
        m_vfobItem.setValue(m_vfob);
        restoreSurvey();
    }
}
void RigControl::setRitOffset(qint32 frequency)
{
    if (m_ritOffset == frequency)
        return;

    pauseSurvey();
    m_ritOffset = frequency;
    m_ritOffsetItem.setValue(m_ritOffset);
    restoreSurvey();
}
void RigControl::setRit(bool enable)
{
    if (m_rit == enable)
        return;

    pauseSurvey();
    m_rit = enable;
    m_ritItem.setValue(m_rit);
    restoreSurvey();
}
void RigControl::setXit(bool enable)
{
    if (m_xit == enable)
        return;

    pauseSurvey();
    m_xit = enable;
    m_xitItem.setValue(m_xit);
    restoreSurvey();
}
void RigControl::setSplit(bool enable)
{
    if (m_split == enable)
        return;

    pauseSurvey();
    m_split = enable;
    m_splitItem.setValue(m_split);
    restoreSurvey();
}
void RigControl::setTrx(bool state)
{
    if (m_trx == state)
        return;

    pauseSurvey();
    m_trx = state;
    m_trxItem.setValue(m_trx);
    restoreSurvey();
}
void RigControl::setMode(RigAbstract::Mode value)
{
    if (m_mode == value)
        return;

    pauseSurvey();
    m_mode = value;
    m_modeItem.setValue(m_mode);
    restoreSurvey();
}

void RigControl::run()
{
    QTimer t_timer;
    pMainTimer = &t_timer;
    connect(&t_timer, &QTimer::timeout, this, &RigControl::onProcess, Qt::DirectConnection);

    // отправляем команды инициализации CAT системы
    onInit();

    // запускаем опрос состояний трансивера
    t_timer.setSingleShot(true);
    t_timer.start(m_timeout);

    // входим в бесконечный цикл
    exec();
}

bool RigControl::onInit()
{
    bool ok = true;
    for (auto &cmds : m_initCmds) {
        emit cmd(cmds.command());

        if (cmds.waitAnswer()) {
            if (m_queue.waitForReadyRead(m_timeout)) {
                m_queue.pop(m_answer);
                ok = cmds.setAnswer(m_answer);
            }
        }

        if (!ok)
            break;

        msleep(m_poll);
    }

    return ok;
}

void RigControl::onProcess()
{
    pMainTimer->stop();

    for (auto &cmds : m_statusCmds) {
        if (m_pause)
            break;

        while (m_queue.count())
            m_queue.pop(m_answer);

        m_acc.clear();
        emit cmd(cmds.command());

        if (cmds.waitAnswer()) {
            while (m_queue.waitForReadyRead(m_timeout)) {
                if (m_pause)
                    break;

                m_queue.pop(m_answer);
                m_acc += m_answer;
                if (!cmds.validAnswer(m_acc))
                    continue;

                cmds.setAnswer(m_acc);
                //processValues(cmds.values());
                //processFlags(cmds.states());
                break;
            }
        }

        msleep(m_poll);
    }

    // установка параметров
    if (m_vfoaItem.changed()) {
        msleep(m_poll);
        // отправляем команду
        if (m_setCmds[CmdAbstract::pmFreqA].cmdId() != CmdAbstract::pmNone)
            emit cmd(m_setCmds[CmdAbstract::pmFreqA].command(static_cast<int>(m_vfoaItem.value())));
        else if (m_setCmds[CmdAbstract::pmFreq].cmdId() != CmdAbstract::pmNone)
            emit cmd(m_setCmds[CmdAbstract::pmFreq].command(static_cast<int>(m_vfoaItem.value())));
    }
    if (m_vfobItem.changed()) {
        msleep(m_poll);
        // отправляем команду
        if (m_setCmds[CmdAbstract::pmFreqB].cmdId() != CmdAbstract::pmNone)
            emit cmd(m_setCmds[CmdAbstract::pmFreqB].command(static_cast<int>(m_vfobItem.value())));
    }
    if (m_ritOffsetItem.changed()) {
        msleep(m_poll);
        // отправляем команду
        if (m_setCmds[CmdAbstract::pmRitOffset].cmdId() != CmdAbstract::pmNone)
            emit cmd(m_setCmds[CmdAbstract::pmRitOffset].command(static_cast<int>(m_ritOffsetItem.value())));
    }
    if (m_ritItem.changed()) {
        msleep(m_poll);
        // отправляем команду
        if (m_ritItem.value()) {
            if (m_setCmds[CmdAbstract::pmRitOn].cmdId() != CmdAbstract::pmNone)
                emit cmd(m_setCmds[CmdAbstract::pmRitOn].command());
        }
        else {
            if (m_setCmds[CmdAbstract::pmRitOff].cmdId() != CmdAbstract::pmNone)
                emit cmd(m_setCmds[CmdAbstract::pmRitOff].command());
        }
    }
    if (m_xitItem.changed()) {
        msleep(m_poll);
        // отправляем команду
        if (m_xitItem.value()) {
            if (m_setCmds[CmdAbstract::pmXitOn].cmdId() != CmdAbstract::pmNone)
                emit cmd(m_setCmds[CmdAbstract::pmXitOn].command());
        }
        else {
            if (m_setCmds[CmdAbstract::pmXitOff].cmdId() != CmdAbstract::pmNone)
                emit cmd(m_setCmds[CmdAbstract::pmXitOff].command());
        }
    }
    if (m_splitItem.changed()) {
        msleep(m_poll);
        // отправляем команду
        if (m_splitItem.value()) {
            if (m_setCmds[CmdAbstract::pmSplitOn].cmdId() != CmdAbstract::pmNone)
                emit cmd(m_setCmds[CmdAbstract::pmSplitOn].command());
        }
        else {
            if (m_setCmds[CmdAbstract::pmSplitOff].cmdId() != CmdAbstract::pmNone)
                emit cmd(m_setCmds[CmdAbstract::pmSplitOff].command());
        }
    }
    if (m_trxItem.changed()) {
        msleep(m_poll);
        // отправляем команду
        if (m_trxItem.value()) {
            if (m_setCmds[CmdAbstract::pmTx].cmdId() != CmdAbstract::pmNone)
                emit cmd(m_setCmds[CmdAbstract::pmTx].command());
        }
        else {
            if (m_setCmds[CmdAbstract::pmRx].cmdId() != CmdAbstract::pmNone)
                emit cmd(m_setCmds[CmdAbstract::pmRx].command());
        }
    }
    if (m_modeItem.changed()) {
        msleep(m_poll);
        // отправляем команду
        switch (m_modeItem.value()) {
            case RigAbstract::AM:
                if (m_setCmds[CmdAbstract::pmAM].cmdId() != CmdAbstract::pmNone)
                    emit cmd(m_setCmds[CmdAbstract::pmAM].command());
                break;

            case RigAbstract::FM:
                if (m_setCmds[CmdAbstract::pmFM].cmdId() != CmdAbstract::pmNone)
                    emit cmd(m_setCmds[CmdAbstract::pmFM].command());
                break;

            case RigAbstract::CWL:
                if (m_setCmds[CmdAbstract::pmCW_L].cmdId() != CmdAbstract::pmNone)
                    emit cmd(m_setCmds[CmdAbstract::pmCW_L].command());
                break;

            case RigAbstract::CWU:
                if (m_setCmds[CmdAbstract::pmCW_U].cmdId() != CmdAbstract::pmNone)
                    emit cmd(m_setCmds[CmdAbstract::pmCW_U].command());
                break;

            case RigAbstract::USB:
                if (m_setCmds[CmdAbstract::pmSSB_U].cmdId() != CmdAbstract::pmNone)
                    emit cmd(m_setCmds[CmdAbstract::pmSSB_U].command());
                break;

            case RigAbstract::LSB:
                if (m_setCmds[CmdAbstract::pmSSB_L].cmdId() != CmdAbstract::pmNone)
                    emit cmd(m_setCmds[CmdAbstract::pmSSB_L].command());
                break;

            case RigAbstract::DIGL:
                if (m_setCmds[CmdAbstract::pmDIG_L].cmdId() != CmdAbstract::pmNone)
                    emit cmd(m_setCmds[CmdAbstract::pmDIG_L].command());
                break;

            case RigAbstract::DIGU:
                if (m_setCmds[CmdAbstract::pmDIG_U].cmdId() != CmdAbstract::pmNone)
                    emit cmd(m_setCmds[CmdAbstract::pmDIG_U].command());
                break;

            default:
                break;
        }
    }

    //
    pMainTimer->setSingleShot(true);
    pMainTimer->start(m_timeout);
}

void RigControl::init()
{
    m_setCmds.resize(CmdAbstract::XmdICount);

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
            m_initCmds << t_cmds;
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
            m_statusCmds << t_cmds;
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

            if (m_mapGroup.contains(group.toLower())) {
                CmdAbstract::CmdId t_id = m_mapGroup[group.toLower()];
                if (t_id > CmdAbstract::pmNone) {
                    if ((t_id == CmdAbstract::pmFreq) || (t_id == CmdAbstract::pmFreqA) || (t_id == CmdAbstract::pmFreqB) || (t_id == CmdAbstract::pmRitOffset))
                        t_cmds.setWorkMode(RigCmds::SetMode);

                    t_cmds.setCmdId(t_id);
                    m_setCmds[t_id] = t_cmds;
                }
            }

        }
    }
}

void RigControl::pauseSurvey()
{
    if (m_pauseTimer.isActive())
        m_pauseTimer.stop();

    m_pause = true;
}

void RigControl::restoreSurvey()
{
    m_pauseTimer.setSingleShot(true);
    m_pauseTimer.start(m_poll + 100);
}

void RigControl::onStartSurvey()
{
    m_pause = false;
}

void RigControl::processValues(const QList<RigValueState> &list)
{
    if (m_pause)
        return;

    for (auto &name : list) {
        if ((name.cmd() == CmdAbstract::pmFreq) || (name.cmd() == CmdAbstract::pmFreqA)) {
            if (m_vfoa != static_cast<quint32>(name.value())) {
                m_vfoa = static_cast<quint32>(name.value());
                emit vfoChanged(0u, static_cast<quint32>(name.value()));
            }
        }
        else if (name.cmd() == CmdAbstract::pmFreqB) {
            if (m_vfob != static_cast<quint32>(name.value())) {
                m_vfob = static_cast<quint32>(name.value());
                emit vfoChanged(1u, static_cast<quint32>(name.value()));
            }
        }
        else if (name.cmd() == CmdAbstract::pmRitOffset) {
            if (m_ritOffset != name.value()) {
                m_ritOffset = name.value();
                emit ritOffsetChanged(name.value());
            }
        }
    }
}

void RigControl::processFlags(const QList<CmdAbstract::CmdId> &list)
{
    if (m_pause)
        return;

    for (auto &cmdId : list) {
        switch (cmdId) {
            case CmdAbstract::pmSplitOn:
                if (!m_split) {
                    m_split = true;
                    emit splitChanged(m_split);
                }
                break;

            case CmdAbstract::pmSplitOff:
                if (m_split) {
                    m_split = false;
                    emit splitChanged(m_split);
                }
                break;

            case CmdAbstract::pmRitOn:
                if (!m_rit) {
                    m_rit = true;
                    emit ritChanged(m_rit);
                }
                break;

            case CmdAbstract::pmRitOff:
                if (m_rit) {
                    m_rit = false;
                    emit ritChanged(m_rit);
                }
                break;

            case CmdAbstract::pmXitOn:
                if (!m_xit) {
                    m_xit = true;
                    emit xitChanged(m_xit);
                }
                break;

            case CmdAbstract::pmXitOff:
                if (m_xit) {
                    m_xit = false;
                    emit xitChanged(m_xit);
                }
                break;

            case CmdAbstract::pmRx:
                if (m_trx) {
                    m_trx = false;
                    emit trxChanged(m_trx);
                }
                break;

            case CmdAbstract::pmTx:
                if (!m_trx) {
                    m_trx = true;
                    emit trxChanged(m_trx);
                }
                break;

            case CmdAbstract::pmCW_U:
                if (m_mode != RigAbstract::CWU) {
                    m_mode = RigAbstract::CWU;
                    emit modeChanged(m_mode);
                }
                break;

            case CmdAbstract::pmCW_L:
                if (m_mode != RigAbstract::CWL) {
                    m_mode = RigAbstract::CWL;
                    emit modeChanged(m_mode);
                }
                break;

            case CmdAbstract::pmSSB_U:
                if (m_mode != RigAbstract::USB) {
                    m_mode = RigAbstract::USB;
                    emit modeChanged(m_mode);
                }
                break;

            case CmdAbstract::pmSSB_L:
                if (m_mode != RigAbstract::LSB) {
                    m_mode = RigAbstract::LSB;
                    emit modeChanged(m_mode);
                }
                break;

            case CmdAbstract::pmDIG_L:
                if (m_mode != RigAbstract::DIGL) {
                    m_mode = RigAbstract::DIGL;
                    emit modeChanged(m_mode);
                }
                break;

            case CmdAbstract::pmDIG_U:
                if (m_mode != RigAbstract::DIGU) {
                    m_mode = RigAbstract::DIGU;
                    emit modeChanged(m_mode);
                }
                break;

            case CmdAbstract::pmAM:
                if (m_mode != RigAbstract::AM) {
                    m_mode = RigAbstract::AM;
                    emit modeChanged(m_mode);
                }
                break;

            case CmdAbstract::pmFM:
                if (m_mode != RigAbstract::FM) {
                    m_mode = RigAbstract::FM;
                    emit modeChanged(m_mode);
                }
                break;

            default:
                break;
        }
    }
}






