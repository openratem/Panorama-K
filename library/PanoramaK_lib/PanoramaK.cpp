#include "PanoramaK.h"
#include "CatTransceiver/CatTransceiver.h"
#include "SwrFilter/SwrFilter.h"
#include "TciTransceiver/TciTransceiver.h"
#include <OmniRig>

namespace PanoramaK {

static QList<Protocol> Protocols;

TransceiverFactory::TransceiverFactory()
{
}

QList<Protocol> TransceiverFactory::devices()
{
    if (Protocols.empty()) {
        Protocols.append({ "Не выбрано", ControlInterface::Unknown });
        Protocols.append({ "SunSDR TCI", ControlInterface::Network });

        for (auto& title : OmniRig::instance().protocols())
            Protocols.append({ title, ControlInterface::SerialPort });
    }

    return Protocols;
}

std::shared_ptr<TransceiverAbstract> TransceiverFactory::create(const QString& device)
{
    const auto itter = std::find_if(Protocols.cbegin(), Protocols.cend(), [device](const Protocol& protocol) { return protocol.name == device; });
    if (itter != Protocols.end()) {
        if (itter->controlInterface == ControlInterface::Network) {
            // протокол SunSDR TCI
            return std::make_shared<TciTransceiver>();
        } else if (itter->controlInterface == ControlInterface::SerialPort) {
            // протокол CAT
            return std::make_shared<CatTransceiver>(device);
        }
    }

    return {};
}

struct Manager::impl {
    impl(Manager& context,
        const std::shared_ptr<PowerSwrAnalizerAbstract>& analizer,
        const std::shared_ptr<TransceiverAbstract>& transceiver)
        : m_context { context }
        , pAnalizer { analizer }
        , pTransceiver { transceiver }
    {
        QObject::connect(pTransceiver.get(), &TransceiverAbstract::connectionStatusChanged, pTransceiver.get(), [&](auto status) {
            if (status == ConnectionStatus::Connected)
                onStatus(Status::Unknown);
        });

        QObject::connect(pAnalizer.get(), &PowerSwrAnalizerAbstract::powerSwrValue, pAnalizer.get(), [&](float watts, float swr) {
            const auto t_filteredSwr = m_swrFilter.process(swr);
            emit m_context.powerSwrValue(watts, swr, t_filteredSwr);
            onStatus(processSwr(t_filteredSwr, m_swrImpulseFilter.process(swr)));
        });

        QObject::connect(pTransceiver.get(), &TransceiverAbstract::trxChanged, pTransceiver.get(), [&](bool txMode) {
            if (txMode) {
                m_swrFilter.reset();
                m_swrImpulseFilter.reset();
                m_status = PanoramaK::Status::Unknown;
                emit m_context.statusChanged(m_status);
            }
        });

        QObject::connect(&m_context, &Manager::badContactDetected, pTransceiver.get(), [&]() {
            pTransceiver->turnOffTransmissionMode();
        });
    }

private:
    PanoramaK::Status processSwr(float swr, float impulse)
    {
        qDebug() << Q_FUNC_INFO << QString("(swr: %1, impulse: %2)").arg(swr).arg(impulse);

        // определяем состояние контактов
        if (impulse > 1.0) {
            emit m_context.badContactDetected();
            return Status(Status::BadContact);
        }

        // определяем общее состояние АФУ
        if (swr <= 1.3f)
            return Status(Status::Ok);
        else if (swr < 1.6f)
            return Status(Status::ValidSwr);
        else if (swr < 2.1f)
            return Status(Status::HighSwr);
        else if (swr <= 5.0f)
            return Status(Status::VeryHighSwr);
        else
            return Status(Status::CableBreakage);
    }

    void onStatus(PanoramaK::Status status)
    {
        if (m_status != status) {
            m_status = status;
            emit m_context.statusChanged(m_status);

            if (m_status > Status::HighSwr)
                pTransceiver->turnOffTransmissionMode();
        }
    }

private:
    Manager& m_context;
    std::shared_ptr<PowerSwrAnalizerAbstract> pAnalizer;
    std::shared_ptr<TransceiverAbstract> pTransceiver;
    SwrFilter m_swrFilter;
    SwrImpulseFilter m_swrImpulseFilter;
    PanoramaK::Status m_status { PanoramaK::Status::Ok };
};

Manager::Manager(const std::shared_ptr<PowerSwrAnalizerAbstract>& analizer,
    const std::shared_ptr<TransceiverAbstract>& transceiver,
    QObject* parent)
    : QObject { parent }
    , pImpl { std::make_unique<impl>(*this, analizer, transceiver) }
{
}

Manager::~Manager()
{
}

std::shared_ptr<PowerSwrAnalizerAbstract> PowerSwrAnalizerFactory::create(const QJsonObject&)
{
    return {};
}

} // namespace PanoramaK
