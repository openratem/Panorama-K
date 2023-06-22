#include "RigSerialPort.h"

RigSerialPort::RigSerialPort(PanoramaK::QueueEvent &queue, QObject *parent) :
  QObject(parent),
  m_open(false),
  m_queue(queue)
{
    connect(&m_port, &QSerialPort::readyRead, this, &RigSerialPort::onRead);
}

QStringList RigSerialPort::availablePorts()
{
    QStringList t_list;
    for (auto &info : QSerialPortInfo::availablePorts())
        t_list << info.portName();
    return t_list;
}

bool RigSerialPort::open(const RigSerialPortParam &param)
{
    if (m_open)
        return true;

    m_status.clear();
    m_param = param;

    m_port.setPortName(m_param.name());
    m_port.setBaudRate(m_param.baudRate());
    m_port.setDataBits(m_param.dataBits());
    m_port.setParity(m_param.parity());
    m_port.setStopBits(m_param.stopBits());
    m_port.setFlowControl(m_param.flowControl());

    m_open = m_port.open(QSerialPort::ReadWrite);
    if (m_open) {
        m_port.setDataTerminalReady(m_param.dtr());
        m_port.setRequestToSend(m_param.rts());
    }

    return m_open;
}
void RigSerialPort::close()
{
    if (!m_open)
        return;

    m_port.close();
    m_open = false;
}
bool RigSerialPort::isOpen() const noexcept
{
    return m_open;
}

QString RigSerialPort::lastError() const
{
    return m_port.errorString();
}

void RigSerialPort::write(const QByteArray &data)
{
    if (!m_open)
        return;

    m_port.write(data);
    m_port.flush();
}

void RigSerialPort::setDtr(bool state)
{
    if (!m_open)
        return;

    m_port.setDataTerminalReady(state);
}
void RigSerialPort::setRts(bool state)
{
    if (!m_open)
        return;

    m_port.setRequestToSend(state);
}

void RigSerialPort::onRead()
{
    QSerialPort *pPort = qobject_cast<QSerialPort*>(sender());
    m_queue.push(pPort->readAll());
}














