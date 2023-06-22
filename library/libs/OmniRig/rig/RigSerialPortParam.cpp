#include "RigSerialPortParam.h"

RigSerialPortParam::RigSerialPortParam() :
  m_rts(false),
  m_dtr(false),
  m_timeout(200u),
  m_pollTimeout(200u),
  m_baudRate(QSerialPort::Baud9600),
  m_dataBits(QSerialPort::Data8),
  m_parity(QSerialPort::NoParity),
  m_stopBits(QSerialPort::OneStop),
  m_flowControl(QSerialPort::NoFlowControl)
{

}
RigSerialPortParam::RigSerialPortParam(const RigSerialPortParam &param) :
  m_rts(param.rts()),
  m_dtr(param.dtr()),
  m_timeout(param.timeout()),
  m_pollTimeout(param.pollTimeout()),
  m_baudRate(param.baudRate()),
  m_dataBits(param.dataBits()),
  m_parity(param.parity()),
  m_stopBits(param.stopBits()),
  m_flowControl(param.flowControl()),
  m_name(param.name())
{

}
RigSerialPortParam&  RigSerialPortParam::operator=(const RigSerialPortParam &param)
{
    m_name        = param.name();
    m_rts         = param.rts();
    m_dtr         = param.dtr();
    m_timeout     = param.timeout();
    m_pollTimeout = param.pollTimeout();
    m_baudRate    = param.baudRate();
    m_dataBits    = param.dataBits();
    m_parity      = param.parity();
    m_stopBits    = param.stopBits();
    m_flowControl = param.flowControl();
    return *this;
}

void RigSerialPortParam::setName(const QString &str) noexcept
{
    m_name = str;
}
QString RigSerialPortParam::name() const noexcept
{
    return m_name;
}

void RigSerialPortParam::setBaudRate(QSerialPort::BaudRate value) noexcept
{
    m_baudRate = qBound(QSerialPort::Baud1200, value, QSerialPort::Baud115200);
}
QSerialPort::BaudRate RigSerialPortParam::baudRate() const noexcept
{
    return m_baudRate;
}

void RigSerialPortParam::setDataBits(QSerialPort::DataBits value) noexcept
{
    m_dataBits = qBound(QSerialPort::Data5, value, QSerialPort::Data8);
}
QSerialPort::DataBits RigSerialPortParam::dataBits() const noexcept
{
    return m_dataBits;
}

void RigSerialPortParam::setParity(QSerialPort::Parity value) noexcept
{
    m_parity = qBound(QSerialPort::NoParity, value, QSerialPort::MarkParity);
}
QSerialPort::Parity RigSerialPortParam::parity() const noexcept
{
    return m_parity;
}

void RigSerialPortParam::setStopBits(QSerialPort::StopBits value) noexcept
{
    m_stopBits = qBound(QSerialPort::OneStop, value, QSerialPort::TwoStop);
}
QSerialPort::StopBits RigSerialPortParam::stopBits() const noexcept
{
    return m_stopBits;
}

void RigSerialPortParam::setFlowControl(QSerialPort::FlowControl value) noexcept
{
    m_flowControl = qBound(QSerialPort::NoFlowControl, value, QSerialPort::SoftwareControl);
}
QSerialPort::FlowControl RigSerialPortParam::flowControl() const noexcept
{
    return m_flowControl;
}

void RigSerialPortParam::setTimeout(quint32 value) noexcept
{
    m_timeout = qBound(10u, value, 1000u);
}
quint32 RigSerialPortParam::timeout() const noexcept
{
    return m_timeout;
}

void RigSerialPortParam::setPollTimeout(quint32 value) noexcept
{
    m_pollTimeout = qBound(10u, value, 1000u);
}
quint32 RigSerialPortParam::pollTimeout() const noexcept
{
    return m_pollTimeout;
}

void RigSerialPortParam::setRts(bool state) noexcept
{
    m_rts = state;
}
bool RigSerialPortParam::rts() const noexcept
{
    return m_rts;
}

void RigSerialPortParam::setDtr(bool state) noexcept
{
    m_dtr = state;
}
bool RigSerialPortParam::dtr() const noexcept
{
    return m_dtr;
}
