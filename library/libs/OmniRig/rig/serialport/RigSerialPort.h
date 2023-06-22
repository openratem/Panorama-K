#ifndef RIGSERIALPORT_H
#define RIGSERIALPORT_H

#include <atomic>
#include <QtCore>
#include <QtSerialPort>

#include "../RigSerialPortParam.h"
#include "../control/queueevent.h"

class RigSerialPort : public QObject
{
    Q_OBJECT

public:
    explicit RigSerialPort(PanoramaK::QueueEvent &queue, QObject *parent = nullptr);

    static QStringList availablePorts();

    bool open(const RigSerialPortParam &param);
    void close();
    bool isOpen() const noexcept;

    QString lastError() const;

public slots:
    void write(const QByteArray &data);

    void setDtr(bool state);
    void setRts(bool state);

signals:
    void readyRead(QByteArray data);

private slots:
    void onRead();

private:
    std::atomic_bool   m_open;
    PanoramaK::QueueEvent &m_queue;
    RigSerialPortParam m_param;
    QString            m_status;
    QSerialPort        m_port;
};

#endif // RIGSERIALPORT_H
