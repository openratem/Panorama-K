#ifndef RIGCONTROL_H
#define RIGCONTROL_H

#include <QtCore>
#include <atomic>
#include "../../RigAbstract.h"
#include "../converter/cmd/RigCmd.h"
#include "../parser/RigParser.h"
#include "queueevent.h"

template <typename T>
class RigControlItem
{
public:
    RigControlItem() noexcept {};

    void setValue(T t_value) noexcept { m_value = t_value; m_changed = true; }
    T value() noexcept { m_changed = false; return m_value; }
    bool changed() const noexcept { return m_changed; }

private:
    Q_DISABLE_COPY(RigControlItem)

private:
    std::atomic_bool m_changed = { false };
    std::atomic<T>   m_value   = { static_cast<T>(0) };
};

class RigControl : public QThread
{
    Q_OBJECT

public:
    explicit RigControl(RigParser &parser, QObject *parent = nullptr);

    void open(quint32 timeout, quint32 poll_ms);
    void close();
    bool isOpen() const;

    PanoramaK::QueueEvent& queue();

    void setVfo(quint32 channel, quint32 frequency);
    void setRitOffset(qint32 frequency);
    void setRit(bool enable);
    void setXit(bool enable);
    void setSplit(bool enable);
    void setTrx(bool state);
    void setMode(RigAbstract::Mode value);

signals:
    void cmd(QByteArray);

    void vfoChanged(quint32 channel, quint32 frequency);
    void ritOffsetChanged(qint32 frequency);
    void ritChanged(bool enable);
    void xitChanged(bool enable);
    void splitChanged(bool enable);
    void trxChanged(bool enable);
    void modeChanged(RigAbstract::Mode value);

private slots:
    bool onInit();
    void onProcess();

    void onStartSurvey();

private:
    void init();

    void pauseSurvey();
    void restoreSurvey();

    void processValues(const QList<RigValueState> &list);
    void processFlags(const QList<CmdAbstract::CmdId> &list);

protected:
    void run() override;

private:
    std::atomic_bool m_open;
    quint32          m_timeout;
    quint32          m_poll;
    std::atomic_bool m_pause;

    RigParser &m_parser;
    QTimer    *pMainTimer;

    std::atomic<quint32> m_vfoa;
    std::atomic<quint32> m_vfob;
    std::atomic<qint32>  m_ritOffset;
    std::atomic_bool     m_rit;
    std::atomic_bool     m_xit;
    std::atomic_bool     m_split;
    std::atomic_bool     m_trx;
    std::atomic<RigAbstract::Mode> m_mode;

    RigControlItem<quint32> m_vfoaItem;
    RigControlItem<quint32> m_vfobItem;
    RigControlItem<qint32>  m_ritOffsetItem;
    RigControlItem<bool>    m_ritItem;
    RigControlItem<bool>    m_xitItem;
    RigControlItem<bool>    m_splitItem;
    RigControlItem<bool>    m_trxItem;
    RigControlItem<RigAbstract::Mode> m_modeItem;

    QByteArray m_answer;
    QByteArray m_acc;
    QList<RigCmds>   m_initCmds;
    QList<RigCmds>   m_statusCmds;
    QVector<RigCmds> m_setCmds;

    QTimer m_pauseTimer;
    PanoramaK::QueueEvent m_queue;

    static const QMap<QString, CmdAbstract::CmdId> m_mapGroup;
};

#endif // RIGCONTROL_H
