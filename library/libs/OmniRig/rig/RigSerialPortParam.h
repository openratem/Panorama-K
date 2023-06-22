#ifndef RIGSERIALPORTPARAM_H
#define RIGSERIALPORTPARAM_H

#include <QtCore>
#include <QtSerialPort>

class RigSerialPortParam
{
public:
    RigSerialPortParam();
    RigSerialPortParam(const RigSerialPortParam &param);
    RigSerialPortParam& operator=(const RigSerialPortParam &a);

    void setName(const QString &str) noexcept;
    QString name() const noexcept;

    void setBaudRate(QSerialPort::BaudRate value) noexcept;
    QSerialPort::BaudRate baudRate() const noexcept;

    void setDataBits(QSerialPort::DataBits value) noexcept;
    QSerialPort::DataBits dataBits() const noexcept;

    void setParity(QSerialPort::Parity value) noexcept;
    QSerialPort::Parity parity() const noexcept;

    void setStopBits(QSerialPort::StopBits value) noexcept;
    QSerialPort::StopBits stopBits() const noexcept;

    void setFlowControl(QSerialPort::FlowControl value) noexcept;
    QSerialPort::FlowControl flowControl() const noexcept;

    void setTimeout(quint32 value) noexcept;
    quint32 timeout() const noexcept;

    void setPollTimeout(quint32 value) noexcept;
    quint32 pollTimeout() const noexcept;

    void setRts(bool state) noexcept;
    bool rts() const noexcept;

    void setDtr(bool state) noexcept;
    bool dtr() const noexcept;

private:
    bool m_rts;
    bool m_dtr;
    quint32 m_timeout;
    quint32 m_pollTimeout;
    QSerialPort::BaudRate    m_baudRate;
    QSerialPort::DataBits    m_dataBits;
    QSerialPort::Parity      m_parity;
    QSerialPort::StopBits    m_stopBits;
    QSerialPort::FlowControl m_flowControl;
    QString m_name;
};

#endif // RIGSERIALPORTPARAM_H
