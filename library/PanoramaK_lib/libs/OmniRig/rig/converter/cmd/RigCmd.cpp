#include "RigCmd.h"
#include <QtEndian>

RigCmdValue::RigCmdValue() :
  m_cmdId(CmdAbstract::pmNone),
  m_format(CmdAbstract::vfNone),
  m_startPos(0),
  m_length(0),
  m_multiply(1),
  m_add(0)
{

}
RigCmdValue::RigCmdValue(const QByteArray &data) :
  m_cmdId(CmdAbstract::pmNone),
  m_format(CmdAbstract::vfNone),
  m_startPos(0),
  m_length(0),
  m_multiply(1),
  m_add(0)
{
    init(data);
}
RigCmdValue::RigCmdValue(const RigCmdValue &value)
{
    m_cmdId    = value.m_cmdId;
    m_format   = value.m_format;
    m_startPos = value.m_startPos;
    m_length   = value.m_length;
    m_multiply = value.m_multiply;
    m_add      = value.m_add;
}
RigCmdValue::RigCmdValue(const RigCmdValue &&value)
{
    m_cmdId    = value.m_cmdId;
    m_format   = value.m_format;
    m_startPos = value.m_startPos;
    m_length   = value.m_length;
    m_multiply = value.m_multiply;
    m_add      = value.m_add;
}
RigCmdValue& RigCmdValue::operator=(const RigCmdValue &value)
{
    m_cmdId    = value.m_cmdId;
    m_format   = value.m_format;
    m_startPos = value.m_startPos;
    m_length   = value.m_length;
    m_multiply = value.m_multiply;
    m_add      = value.m_add;
    return *this;
}
RigCmdValue& RigCmdValue::operator=(const RigCmdValue &&value)
{
    m_cmdId    = value.m_cmdId;
    m_format   = value.m_format;
    m_startPos = value.m_startPos;
    m_length   = value.m_length;
    m_multiply = value.m_multiply;
    m_add      = value.m_add;
    return *this;
}
void RigCmdValue::init(const QByteArray &data)
{
    QList<QByteArray> t_list = data.split('|');
    if ((t_list.count() < MinParam) || (t_list.count() > MaxParam))
        return;

    m_cmdId    = CmdAbstract::pmNone;
    m_startPos = t_list.at(RcvStartPos).toUInt();
    m_length   = t_list.at(RcvLength).toUInt();
    m_multiply = t_list.at(RcvMultiply).toDouble();
    m_add      = t_list.at(RcvAdd).toDouble();
    m_format   = getFormat(t_list.at(RcvFormat));

    if (t_list.count() == MaxParam)
        m_cmdId = getName(t_list.last());
}
RigValueState RigCmdValue::checkAnswer(const QByteArray &data, bool &ok)
{
    ok = false;
    int t_value = 0;
    RigValueState t_state;
    QByteArray    t_data;

    switch (m_format) {
        case CmdAbstract::vfText:
            t_data  = QByteArray(&data.data()[m_startPos], m_length).replace(0, '0');
            t_value = t_data.toInt(&ok);
            if (ok)
                t_state.init(m_cmdId, static_cast<int>(t_value*m_multiply + m_add));
            break;

        case CmdAbstract::vfBinL:
            memcpy(&t_value, &data.data()[m_startPos], m_length);
            t_state.init(m_cmdId, static_cast<int>(t_value*m_multiply + m_add));
            ok = true;
            break;

        case CmdAbstract::vfBinB:
            memcpy(&t_value, &data.data()[m_startPos], m_length);
            t_value = qbswap(t_value);
            t_state.init(m_cmdId, static_cast<int>(t_value*m_multiply + m_add));
            ok = true;
            break;

        case CmdAbstract::vfBcdLU:
            if (m_length == 4) {
                quint32 t_value = 0;
                memcpy(&t_value, &data.data()[m_startPos], m_length);
                t_value = QByteArray::number(t_value, 16).toInt();
                t_state.init(m_cmdId, static_cast<int>(t_value*m_multiply + m_add));
                ok = true;
            }
            else if (m_length == 5) {
                quint64 t_value = 0;
                memcpy(&t_value, &data.data()[m_startPos], m_length);
                t_value = QByteArray::number(t_value, 16).toInt();
                t_state.init(m_cmdId, static_cast<int>(t_value*m_multiply + m_add));
                ok = true;
            }
            break;

        case CmdAbstract::vfBcdLS:
            break;

        case CmdAbstract::vfBcdBU:
            if (m_length == 4) {
                quint32 t_value = 0;
                memcpy(&t_value, &data.data()[m_startPos], m_length);
                t_value = QByteArray::number(qbswap(t_value), 16).toInt();
                t_state.init(m_cmdId, static_cast<int>(t_value*m_multiply + m_add));
                ok = true;
            }
            else if (m_length == 5) {
                quint64 t_value = 0;
                memcpy(&t_value, &data.data()[m_startPos], m_length);
                t_value = qbswap(t_value) >> 24;
                t_value = QByteArray::number(t_value, 16).toInt();
                t_state.init(m_cmdId, static_cast<int>(t_value*m_multiply + m_add));
                ok = true;
            }
            break;

        case CmdAbstract::vfBcdBS:
            break;

        case CmdAbstract::vfYaesu:
            if (m_length == 2) {
                quint16 t_val = 0;
                memcpy(&t_val, &data.data()[m_startPos], 2);
                if (t_val & 0x8000) {
                    // отрицательное число
                    t_val &= 0x7FFF;
                    t_val = qbswap(t_val);
                    t_value = -static_cast<qint32>(t_val);
                }
                else {
                    // положительное число
                    t_value = qbswap(t_val);
                }
                t_state.init(m_cmdId, static_cast<int>(t_value*m_multiply + m_add));
                ok = true;
            }
            break;

        default:
            break;
    }

    return t_state;
}
CmdAbstract::CmdId RigCmdValue::cmd() const noexcept
{
    return m_cmdId;
}
CmdAbstract::NumberFormat RigCmdValue::format() const noexcept
{
    return m_format;
}
quint32 RigCmdValue::startPos() const noexcept
{
    return m_startPos;
}
quint32 RigCmdValue::length() const noexcept
{
    return m_length;
}
double RigCmdValue::add() const noexcept
{
    return m_add;
}
double RigCmdValue::multiply() const noexcept
{
    return m_multiply;
}
CmdAbstract::NumberFormat RigCmdValue::getFormat(const QByteArray &data)
{
    QString t_str = QString(data).toLower();

    if (QString("vfText").toLower() == t_str)
        return CmdAbstract::vfText;
    else if (QString("vfBinL").toLower() == t_str)
        return CmdAbstract::vfBinL;
    else if (QString("vfBinB").toLower() == t_str)
        return CmdAbstract::vfBinB;
    else if (QString("vfBcdLU").toLower() == t_str)
        return CmdAbstract::vfBcdLU;
    else if (QString("vfBcdLS").toLower() == t_str)
        return CmdAbstract::vfBcdLS;
    else if (QString("vfBcdBU").toLower() == t_str)
        return CmdAbstract::vfBcdBU;
    else if (QString("vfBcdBS").toLower() == t_str)
        return CmdAbstract::vfBcdBS;
    else if (QString("vfYaesu").toLower() == t_str)
        return CmdAbstract::vfYaesu;
    return CmdAbstract::vfNone;
}
CmdAbstract::CmdId RigCmdValue::getName(const QByteArray &data)
{
    QString t_str = QString(data).toLower();

    if (QString("pmFreqA").toLower() == t_str)
        return CmdAbstract::pmFreqA;
    else if (QString("pmFreqB").toLower() == t_str)
        return CmdAbstract::pmFreqB;
    else if (QString("pmFreq").toLower() == t_str)
        return CmdAbstract::pmFreq;
    else if (t_str.contains("pmRit", Qt::CaseInsensitive))
        return CmdAbstract::pmRitOffset;

    return CmdAbstract::pmNone;
}
qint32 RigCmdValue::toBcd(qint32 dec)
{
    bool ok = false;
    QByteArray t_num = QByteArray::number(qAbs(dec));
    t_num.insert(0, 8 - t_num.size(), '0');

    qint32 t_value = 0;
    char *pValue = reinterpret_cast<char*>(&t_value);

    pValue[0] = (dec < 0) ? 0xFF : QByteArray(t_num.data(), 2).toInt(&ok, 16);
    pValue[1] = QByteArray(&t_num.data()[2], 2).toInt(&ok, 16);
    pValue[2] = QByteArray(&t_num.data()[4], 2).toInt(&ok, 16);
    pValue[3] = QByteArray(&t_num.data()[6], 2).toInt(&ok, 16);

    return t_value;
}
qint32 RigCmdValue::toDec(qint32 bcd)
{
    QByteArray t_num;
    quint8 *pValue1 = reinterpret_cast<quint8*>(&bcd);

    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[3]), 16));
    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[2]), 16));
    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[1]), 16));

    if (pValue1[0] == 0xFF)
        return -t_num.toInt();

    return t_num.toInt();
}
quint32 RigCmdValue::toBcdU(quint32 dec)
{
    bool ok = false;
    QByteArray t_num = QByteArray::number(dec);
    t_num.insert(0, 8 - t_num.size(), '0');

    qint32 t_value = 0;
    char *pValue = reinterpret_cast<char*>(&t_value);

    pValue[0] = QByteArray(t_num.data(), 2).toInt(&ok, 16);
    pValue[1] = QByteArray(&t_num.data()[2], 2).toInt(&ok, 16);
    pValue[2] = QByteArray(&t_num.data()[4], 2).toInt(&ok, 16);
    pValue[3] = QByteArray(&t_num.data()[6], 2).toInt(&ok, 16);

    return t_value;
}
quint32 RigCmdValue::toDecU(quint32 bcd)
{
    QByteArray t_num;
    quint8 *pValue1 = reinterpret_cast<quint8*>(&bcd);

    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[3]), 16));
    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[2]), 16));
    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[1]), 16));
    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[0]), 16));

    return t_num.toUInt();
}
quint32 RigCmdValue::toDecU(quint64 bcd)
{
    QByteArray t_num;
    quint8 *pValue1 = reinterpret_cast<quint8*>(&bcd);

    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[4]), 16));
    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[3]), 16));
    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[2]), 16));
    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[1]), 16));
    t_num.insert(0, QByteArray::number(static_cast<uint>(pValue1[0]), 16));

    return t_num.toUInt();
}


RigCmdFlag::RigCmdFlag() :
  asciiMode(false),
  m_cmdId(CmdAbstract::pmNone)
{

}
RigCmdFlag::RigCmdFlag(const QByteArray &data) :
  asciiMode(false),
  m_cmdId(CmdAbstract::pmNone)
{
    init(data);
}
RigCmdFlag::RigCmdFlag(const RigCmdFlag &flag)
{
    asciiMode = flag.asciiMode;
    m_cmdId   = flag.m_cmdId;
    m_equals  = flag.m_equals;
    m_mask    = flag.m_mask;
}
RigCmdFlag::RigCmdFlag(const RigCmdFlag &&flag)
{
    asciiMode = flag.asciiMode;
    m_cmdId   = flag.m_cmdId;
    m_equals  = flag.m_equals;
    m_mask    = flag.m_mask;
}
RigCmdFlag& RigCmdFlag::operator=(const RigCmdFlag &flag)
{
    asciiMode = flag.asciiMode;
    m_cmdId   = flag.m_cmdId;
    m_equals  = flag.m_equals;
    m_mask    = flag.m_mask;
    return *this;
}
RigCmdFlag& RigCmdFlag::operator=(const RigCmdFlag &&flag)
{
    asciiMode = flag.asciiMode;
    m_cmdId   = flag.m_cmdId;
    m_equals  = flag.m_equals;
    m_mask    = flag.m_mask;
    return *this;
}
void RigCmdFlag::init(const QByteArray &data)
{
    QList<QByteArray> t_list = data.split('|');
    if ((t_list.count() < MinParam) && (t_list.count() > MaxParam))
        return;

    m_cmdId  = getName(t_list.last());
    if (data.contains('(')) {
        // ASCII код
        asciiMode = true;
        m_equals = QString(t_list.at(t_list.count()-2)).remove("(").remove(")").toLatin1();
    }
    else {
        // HEX коды
        asciiMode = false;
        m_equals = fromHexStr(QString(t_list.at(t_list.count()-2)).remove(".").toLatin1());
        if (t_list.count() == MaxParam)
            m_mask = fromHexStr(QString(t_list.first()).remove(".").toLatin1());
    }
}
bool RigCmdFlag::checkAnswer(const QByteArray &data)
{
    bool ok = true;
    if (asciiMode) {
        for (int i = 0; i < data.size(); ++i) {
            if (m_equals.at(i) == '.')
                continue;

            if (data[i] != m_equals.at(i)) {
                ok = false;
                break;
            }
        }
    }
    else {
        if (m_mask.isEmpty()) {
            for (int i = 0; i < data.size(); ++i) {
                if (m_equals.at(i) == 0)
                    continue;

                if (data.at(i) != m_equals.at(i)) {
                    ok = false;
                    break;
                }
            }
        }
        else {
            QByteArray t_temp = data;
            for (int i = 0; i < data.size(); ++i) {
                if ((m_mask.at(i) & t_temp.at(i)) != m_equals.at(i)) {
                    ok = false;
                    break;
                }
            }
        }
    }

    return ok;
}
CmdAbstract::CmdId RigCmdFlag::cmd() const noexcept
{
    return m_cmdId;
}
CmdAbstract::CmdId RigCmdFlag::getName(const QByteArray &data)
{
    QString t_str = QString(data).toLower();
    if (QString("pmSplitOn").toLower() == t_str)
        return CmdAbstract::pmSplitOn;
    else if (QString("pmSplitOff").toLower() == t_str)
        return CmdAbstract::pmSplitOff;
    else if (QString("pmVfoA").toLower() == t_str)
        return CmdAbstract::pmVfoA;
    else if (QString("pmVfoB").toLower() == t_str)
        return CmdAbstract::pmVfoB;
    else if (QString("pmVfoEqual").toLower() == t_str)
        return CmdAbstract::pmVfoEqual;
    else if (QString("pmVfoSwap").toLower() == t_str)
        return CmdAbstract::pmVfoSwap;
    else if (QString("pmVfoAA").toLower() == t_str)
        return CmdAbstract::pmVfoAA;
    else if (QString("pmVfoAB").toLower() == t_str)
        return CmdAbstract::pmVfoAB;
    else if (QString("pmVfoBA").toLower() == t_str)
        return CmdAbstract::pmVfoBA;
    else if (QString("pmVfoBB").toLower() == t_str)
        return CmdAbstract::pmVfoBB;
    else if (QString("pmRitOn").toLower() == t_str)
        return CmdAbstract::pmRitOn;
    else if (QString("pmRitOff").toLower() == t_str)
        return CmdAbstract::pmRitOff;
    else if (QString("pmXitOn").toLower() == t_str)
        return CmdAbstract::pmXitOn;
    else if (QString("pmXitOff").toLower() == t_str)
        return CmdAbstract::pmXitOff;
    else if (QString("pmRx").toLower() == t_str)
        return CmdAbstract::pmRx;
    else if (QString("pmTx").toLower() == t_str)
        return CmdAbstract::pmTx;
    else if (QString("pmCW_U").toLower() == t_str)
        return CmdAbstract::pmCW_U;
    else if (QString("pmCW_L").toLower() == t_str)
        return CmdAbstract::pmCW_L;
    else if (QString("pmSSB_U").toLower() == t_str)
        return CmdAbstract::pmSSB_U;
    else if (QString("pmSSB_L").toLower() == t_str)
        return CmdAbstract::pmSSB_L;
    else if (QString("pmDIG_U").toLower() == t_str)
        return CmdAbstract::pmDIG_U;
    else if (QString("pmDIG_L").toLower() == t_str)
        return CmdAbstract::pmDIG_L;
    else if (QString("pmAM").toLower() == t_str)
        return CmdAbstract::pmAM;
    else if (QString("pmFM").toLower() == t_str)
        return CmdAbstract::pmFM;
    return CmdAbstract::pmNone;
}
QByteArray RigCmdFlag::fromHexStr(const QByteArray &data)
{
    if (data.size() % 2)
        return QByteArray();

    QByteArray t_data; t_data.resize(data.size()/2);
    bool ok = false;
    const char *p = data.data();
    for (qint32 i = 0, j = 0; i < data.size(); i += 2, ++j)
        t_data[j] = static_cast<char>(QByteArray(&p[i], 2).toInt(&ok, 16));
    return t_data;
}



RigValueState::RigValueState() :
  m_cmdId(CmdAbstract::pmNone),
  m_value(0)
{

}
RigValueState::RigValueState(CmdAbstract::CmdId cmd, int value) :
  m_cmdId(cmd),
  m_value(value)
{

}
RigValueState::RigValueState(const RigValueState &flag)
{
    m_cmdId = flag.m_cmdId;
    m_value = flag.m_value;
}
RigValueState::RigValueState(const RigValueState &&flag)
{
    m_cmdId = flag.m_cmdId;
    m_value = flag.m_value;
}
RigValueState& RigValueState::operator=(const RigValueState &flag)
{
    m_cmdId = flag.m_cmdId;
    m_value = flag.m_value;
    return *this;
}
RigValueState& RigValueState::operator=(const RigValueState &&flag)
{
    m_cmdId = flag.m_cmdId;
    m_value = flag.m_value;
    return *this;
}
void RigValueState::init(CmdAbstract::CmdId cmd, int value) noexcept
{
    m_cmdId = cmd;
    m_value = value;
}
CmdAbstract::CmdId RigValueState::cmd() const noexcept
{
    return m_cmdId;
}
int RigValueState::value() const noexcept
{
    return m_value;
}



RigCmds::RigCmds() :
  m_asciiMode(false),
  m_cmdId(pmNone),
  m_format(vfNone),
  m_replyLength(0),
  m_replyEnd(0),
  m_workMode(GetMode)
{

}
RigCmds::RigCmds(const RigCmds &rigCmds)
{
    m_asciiMode   = rigCmds.m_asciiMode;
    m_cmdId       = rigCmds.m_cmdId;
    m_format      = rigCmds.m_format;
    m_replyLength = rigCmds.m_replyLength;
    m_replyEnd    = rigCmds.m_replyEnd;
    m_workMode    = rigCmds.m_workMode;
    m_command     = rigCmds.m_command;
    m_validate    = rigCmds.m_validate;
    m_values      = rigCmds.m_values;
    m_flags       = rigCmds.m_flags;
}
RigCmds::RigCmds(const RigCmds &&rigCmds)
{
    m_asciiMode   = rigCmds.m_asciiMode;
    m_cmdId       = rigCmds.m_cmdId;
    m_format      = rigCmds.m_format;
    m_replyLength = rigCmds.m_replyLength;
    m_replyEnd    = rigCmds.m_replyEnd;
    m_workMode    = rigCmds.m_workMode;
    m_command     = rigCmds.m_command;
    m_validate    = rigCmds.m_validate;
    m_values      = rigCmds.m_values;
    m_flags       = rigCmds.m_flags;
}
RigCmds& RigCmds::operator=(const RigCmds &rigCmds)
{
    m_asciiMode   = rigCmds.m_asciiMode;
    m_cmdId       = rigCmds.m_cmdId;
    m_format      = rigCmds.m_format;
    m_replyLength = rigCmds.m_replyLength;
    m_replyEnd    = rigCmds.m_replyEnd;
    m_workMode    = rigCmds.m_workMode;
    m_command     = rigCmds.m_command;
    m_validate    = rigCmds.m_validate;
    m_values      = rigCmds.m_values;
    m_flags       = rigCmds.m_flags;
    return *this;
}
RigCmds& RigCmds::operator=(const RigCmds &&rigCmds)
{
    m_asciiMode   = rigCmds.m_asciiMode;
    m_cmdId       = rigCmds.m_cmdId;
    m_format      = rigCmds.m_format;
    m_replyLength = rigCmds.m_replyLength;
    m_replyEnd    = rigCmds.m_replyEnd;
    m_workMode    = rigCmds.m_workMode;
    m_command     = rigCmds.m_command;
    m_validate    = rigCmds.m_validate;
    m_values      = rigCmds.m_values;
    m_flags       = rigCmds.m_flags;
    return *this;
}
void RigCmds::setWorkMode(WorkMode mode) noexcept
{
    m_workMode = mode;
}
void RigCmds::setCmdId(CmdId id) noexcept
{
    m_cmdId = id;
}
RigCmds::CmdId RigCmds::cmdId() const noexcept
{
    return m_cmdId;
}
void RigCmds::initParam(TypeParam type, const QByteArray &data)
{
    bool ok = false;
    switch (type) {
        case Command:
            if (data.contains('(')) {
                m_asciiMode = true;
                m_command = QString(data).remove("(").remove(")").toLatin1();
            }
            else {
                m_asciiMode = false;
                m_command = fromHexStr(QString(data).remove(".").toLatin1());
            }
            break;

        case ReplyLength:
            m_replyLength = data.toInt(&ok);
            break;

        case ReplyEnd:
            if (data.contains('(')) {
                m_replyEnd = QString(data).remove("(").remove(")").toLatin1().at(0);
            }
            else {
                m_replyEnd = static_cast<char>(data.toInt(&ok, 16));
            }
            break;

        case Validate:
            if (data.contains('(')) {
                m_asciiMode = true;
                m_validate = QString(data).remove("(").remove(")").toLatin1();
            }
            else {
                m_asciiMode = false;
                m_validate = fromHexStr(QString(data).remove(".").toLatin1());
            }
            break;

        case Value:
            m_values.emplace_back(RigCmdValue(data));
            break;

        case Flag:
            m_flags.emplace_back(RigCmdFlag(data));
            break;

        default:
            break;
    }
}
QByteArray RigCmds::command(int value)
{
    if (m_workMode == GetMode)
        return m_command;

    if (m_asciiMode) {
        QByteArray t_number = QByteArray::number(static_cast<qlonglong>(value*m_values.front().multiply() + m_values.front().add()));
        t_number.insert(0, m_values.front().length() - static_cast<quint32>(t_number.size()), '0');

        QByteArray t_cmd = m_command;
        t_cmd.replace(m_values.front().startPos(), t_number.size(), t_number);

        return t_cmd;
    }
    else {
        if (m_values.empty())
			return m_command;

        switch (m_values.front().format())
        {
            case CmdAbstract::vfText:
            {
                QByteArray t_number = QByteArray::number(static_cast<qlonglong>(value*m_values.front().multiply() + m_values.front().add()));
                t_number.insert(0, m_values.front().length() - static_cast<quint32>(t_number.size()), 0x00);

                QByteArray t_cmd = m_command;
                t_cmd.replace(m_values.front().startPos(), t_number.size(), t_number);
                return t_cmd;
            }
            break;

            case CmdAbstract::vfBinL:
            {
                qint32 t_value = static_cast<qint32>(value);
                QByteArray t_cmd = m_command;
                memcpy(&t_cmd.data()[m_values.front().startPos()], &t_value, m_values.front().length());
                return t_cmd;
            }
            break;

            case CmdAbstract::vfBinB:
            {
                qint32 t_value = qbswap(static_cast<qint32>(value));
                QByteArray t_cmd = m_command;
                memcpy(&t_cmd.data()[m_values.front().startPos()], &t_value, m_values.front().length());
                return t_cmd;
            }
            break;

            case CmdAbstract::vfBcdLU:
            {
                quint32 t_value = qbswap(RigCmdValue::toBcdU(static_cast<quint32>(value*m_values.front().multiply() + m_values.front().add())));
                QByteArray t_cmd = m_command;
                memcpy(&t_cmd.data()[m_values.front().startPos()], &t_value, m_values.front().length());
                return t_cmd;
            }
            break;

            case CmdAbstract::vfBcdLS:
            {
                qint32 t_value = qbswap(RigCmdValue::toBcd(static_cast<quint32>(value*m_values.front().multiply() + m_values.front().add())));

                if (m_values.front().length() == 3)
                    t_value |= (t_value & 0xFF000000) >> 8;

                QByteArray t_cmd = m_command;
                memcpy(&t_cmd.data()[m_values.front().startPos()], &t_value, m_values.front().length());
                return t_cmd;
            }
            break;

            case CmdAbstract::vfBcdBU:
            {
                quint32 t_value = RigCmdValue::toBcdU(static_cast<quint32>(value*m_values.front().multiply() + m_values.front().add()));
                QByteArray t_cmd = m_command;
                memcpy(&t_cmd.data()[m_values.front().startPos()], &t_value, m_values.front().length());
                return t_cmd;
            }
            break;

            case CmdAbstract::vfBcdBS:
            {
                qint32 t_value = RigCmdValue::toBcd(static_cast<qint32>(value*m_values.front().multiply() + m_values.front().add()));
                QByteArray t_cmd = m_command;
                memcpy(&t_cmd.data()[m_values.front().startPos()], &t_value, m_values.front().length());
                return t_cmd;
            }
            break;

            case CmdAbstract::vfYaesu:
                break;

            default:
                break;
        }
    }

    return QByteArray();
}

bool RigCmds::waitAnswer() noexcept
{
    if ((m_replyLength > 0u) || (m_replyEnd != 0))
        return true;
    return false;
}

bool RigCmds::setAnswer(const QByteArray &data)
{
    m_valueStates.clear();
    m_states.clear();

    if (!validate(data))
        return false;

    bool ok = false;
    RigValueState t_value;

    for (auto &v : m_values) {
        t_value = v.checkAnswer(data, ok);
        if (ok)
            m_valueStates.push_back(t_value);
    }

    for (auto &f : m_flags) {
        if (f.checkAnswer(data))
            m_states.push_back(f.cmd());
    }

    return true;
}
bool RigCmds::validAnswer(const QByteArray &data)
{
	if (m_asciiMode) {
		if (!m_validate.isEmpty())
			return data.size() == m_validate.size();
	}

	if (m_replyLength)
		return m_replyLength == static_cast<quint32>(data.size());

	if (m_replyEnd && !data.isEmpty())
        return data[data.size() - 1] == m_replyEnd;

	return true;
}
gsl::span<RigValueState> RigCmds::values()
{
    return m_valueStates;
}
gsl::span<CmdAbstract::CmdId> RigCmds::states()
{
    return m_states;
}
QByteArray RigCmds::fromHexStr(const QByteArray &data)
{
    if (data.size() % 2)
        return QByteArray();

    QByteArray t_data; t_data.resize(data.size()/2);
    bool ok = false;
    const char *p = data.data();
    for (qint32 i = 0, j = 0; i < data.size(); i += 2, ++j)
        t_data[j] = static_cast<char>(QByteArray(&p[i], 2).toInt(&ok, 16));
    return t_data;
}
bool RigCmds::validate(const QByteArray &data)
{
    if (m_validate.isEmpty()) {
        if (m_replyEnd) {
            if (data.at(data.size() - 1) == m_replyEnd)
                return true;
            return false;
        }
        return true;
    }

    if (m_validate.size() != data.size())
        return false;

    bool ok = false;
    if (m_asciiMode) {
        ok = true;
        for (int i = 0; i < data.size(); ++i) {
            if (m_validate.at(i) == '.')
                continue;

            if (data[i] != m_validate.at(i)) {
                ok = false;
                break;
            }
        }
    }

    return ok;
}





























