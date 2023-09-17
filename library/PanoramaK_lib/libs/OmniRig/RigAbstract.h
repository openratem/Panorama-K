#pragma once

#include <QtCore>

/**
 * \class RigAbstract
 * \brief Интерфейс взаимодействия между программой трансивером, через COM порт.
 */
class RigAbstract : public QObject
{
    Q_OBJECT

public:
    /**
     * \enum Mode
     * \brief Список модуляций.
     */
    typedef enum
    {
        AM = 0,//!< AM
        FM,    //!< FM
        LSB,   //!< LSB
        USB,   //!< USB
        CWL,   //!< CWL
        CWU,   //!< CWU
        DIGL,  //!< DIGL
        DIGU   //!< DIGU
    } Mode;

public:
    explicit RigAbstract(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~RigAbstract() {}

public slots:
    virtual void open(quint32 polling, quint32 timeout) = 0;

    virtual void setVfo(quint32 channel, quint32 frequency) = 0;

    virtual void setRitOffset(qint32 frequency) = 0;

    virtual void setRit(bool enable) = 0;

    virtual void setXit(bool enable) = 0;

    virtual void setSplit(bool enable) = 0;

    virtual void setTrx(bool enable) = 0;

    virtual void setMode(Mode value) = 0;

signals:
    void connected();

    void disconnected();

    void vfoChanged(quint32 channel, quint32 frequency);

    void ritOffsetChanged(qint32 frequency);

    void ritChanged(bool enable);

    void xitChanged(bool enable);

    void splitChanged(bool enable);

    void trxChanged(bool enable);

    void modeChanged(Mode value);
};

