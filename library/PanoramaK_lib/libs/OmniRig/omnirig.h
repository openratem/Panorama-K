#pragma once

#include <memory>
#include <QString>
#include <QObject>
#include <QSerialPort>
#include "RigAbstract.h"


namespace PanoramaK {

class OmniRig final : public QObject
{
    Q_OBJECT

public:
    /**
     * \brief При первом вызове создаёт экземпляр класса OmniRig и возвращает ссылку на него,
     * при последующих вызовах просто возвращает ссылку на уже существующий класс.
     * \return ссылку на экземпляр класса OmniRig.
     */
    static OmniRig& instance();

    /**
     * \brief Возвращает список поддерживаемых протоколов.
     * \return список поддерживаемых протоколов.
     */
    QStringList protocols() const noexcept;

    /**
     * \brief Создаёт обект управления радио устройством.
     * \param serialPort - COM порт.
     * \param protocol - протокол радио.
     */
    std::unique_ptr<RigAbstract> createRig(QSerialPort &serialPort, const QString &protocol) const;

private:
    void search();

private:
    explicit OmniRig(QObject *parent = nullptr);
    Q_DISABLE_COPY(OmniRig)

private:
    QStringList m_files;
    QStringList m_protocols;
};

}  // namespace PanoramaK

