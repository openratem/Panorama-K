#pragma once

#include <vector>
#include <QList>
#include <QVector>
#include <QByteArray>
#include <gsl/span>
#include "../CmdAbstract.h"


class RigValueState
{
public:
    RigValueState();
    RigValueState(CmdAbstract::CmdId cmd, int value);
    RigValueState(const RigValueState &flag);
    RigValueState(const RigValueState &&flag);
    RigValueState& operator=(const RigValueState &flag);
    RigValueState& operator=(const RigValueState &&flag);

    void init(CmdAbstract::CmdId cmd, int value) noexcept;

    CmdAbstract::CmdId cmd() const noexcept;
    int value() const noexcept;

private:
    CmdAbstract::CmdId m_cmdId;
    int m_value;
};


/**
 * \class RigCmdValue
 *
 * \details
 * Value<N>=<start_pos>|<length>|<format>|<multiply>|<add>[|<param_name>]
 */
class RigCmdValue
{
    static constexpr int MinParam = 5;
    static constexpr int MaxParam = 6;

    typedef enum
    {
        RcvStartPos = 0,
        RcvLength,
        RcvFormat,
        RcvMultiply,
        RcvAdd,
        RcvName
    }ParamId;

public:
    RigCmdValue();
    RigCmdValue(const QByteArray &data);
    RigCmdValue(const RigCmdValue &value);
    RigCmdValue(const RigCmdValue &&value);
    RigCmdValue& operator=(const RigCmdValue &value);
    RigCmdValue& operator=(const RigCmdValue &&value);

    void init(const QByteArray &data);

    RigValueState checkAnswer(const QByteArray &data, bool &ok);

    bool textFormat() { return m_format == CmdAbstract::vfText; }

    CmdAbstract::CmdId cmd() const noexcept;
    CmdAbstract::NumberFormat format() const noexcept;
    quint32 startPos() const noexcept;
    quint32 length() const noexcept;
    double add() const noexcept;
    double multiply() const noexcept;

    static qint32 toBcd(qint32 dec);
    static qint32 toDec(qint32 bcd);

    static quint32 toBcdU(quint32 dec);
    static quint32 toDecU(quint32 bcd);
    static quint32 toDecU(quint64 bcd);

private:
    CmdAbstract::NumberFormat getFormat(const QByteArray &data);
    CmdAbstract::CmdId getName(const QByteArray &data);

private:
    CmdAbstract::CmdId        m_cmdId;
    CmdAbstract::NumberFormat m_format;
    quint32 m_startPos;
    quint32 m_length;
    double  m_multiply;
    double  m_add;

};

/**
 * \class RigCmdFlag
 *
 * \details
 * Flag<N>=<mask>[|<equals>]|<param_name>
 */
class RigCmdFlag
{
    static constexpr int MinParam = 2;
    static constexpr int MaxParam = 3;

public:
    RigCmdFlag();
    RigCmdFlag(const QByteArray &data);
    RigCmdFlag(const RigCmdFlag &flag);
    RigCmdFlag(const RigCmdFlag &&flag);
    RigCmdFlag& operator=(const RigCmdFlag &flag);
    RigCmdFlag& operator=(const RigCmdFlag &&flag);

    void init(const QByteArray &data);

    bool checkAnswer(const QByteArray &data);

    CmdAbstract::CmdId cmd() const noexcept;

private:
    CmdAbstract::CmdId getName(const QByteArray &data);

    QByteArray fromHexStr(const QByteArray &data);

private:
    bool asciiMode;
    CmdAbstract::CmdId m_cmdId;
    QByteArray m_equals;
    QByteArray m_mask;
};


/**
 * \class RigCmds
 *
 *
 * \details
 * Секция INIT содержит поля: 'COMMAND', 'REPLYLENGTH', 'REPLYEND', 'VALIDATE'
 * Секции команд записи параметров содержат поля: 'COMMAND', 'REPLYLENGTH', 'REPLYEND', 'VALIDATE', 'VALUE'
 * Секция STATUS содержит: 'COMMAND', 'REPLYLENGTH', 'REPLYEND', 'VALIDATE', 'VALUE*', 'FLAG*'
 *
 */
class RigCmds : public CmdAbstract
{
public:
    typedef enum
    {
        Unknown = -1,
        Command,
        ReplyLength,
        ReplyEnd,
        Validate,
        Value,
        Flag
    }TypeParam;

    typedef enum
    {
        SetMode = 0,
        GetMode
    }WorkMode;

public:
    RigCmds();
    explicit RigCmds(const RigCmds &rigCmds);
    RigCmds(const RigCmds &&rigCmds);
    RigCmds& operator=(const RigCmds &a);
    RigCmds& operator=(const RigCmds &&a);

    void setWorkMode(WorkMode mode) noexcept;

    void setCmdId(CmdId id) noexcept;
    CmdId cmdId() const noexcept;

    void initParam(TypeParam type, const QByteArray &data);

    QByteArray command(int value = 0);

    bool waitAnswer() noexcept;

    bool textFormat() const { return m_asciiMode; }

    bool validAnswer(const QByteArray &data);
    bool setAnswer(const QByteArray &data);

    gsl::span<RigValueState> values();
    gsl::span<CmdAbstract::CmdId> states();

private:
    QByteArray fromHexStr(const QByteArray &data);
    bool validate(const QByteArray &data);

private:
    bool         m_asciiMode;
    CmdId        m_cmdId;
    NumberFormat m_format;
    quint32      m_replyLength;
    char         m_replyEnd;
    WorkMode     m_workMode;
    QByteArray   m_command;
    QByteArray   m_validate;
    std::vector<RigCmdValue> m_values;
    std::vector<RigCmdFlag>  m_flags;
    std::vector<RigValueState> m_valueStates;
    std::vector<CmdAbstract::CmdId> m_states;
};





























