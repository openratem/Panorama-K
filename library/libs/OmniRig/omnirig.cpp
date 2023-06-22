#include "omnirig.h"
#include "rig/Rig.h"

namespace PanoramaK {

static QStringList Devices
{
    "Elecraft K2.ini",
    "Elecraft K3.ini",
    "Elecraft KX3.ini",
    "FT-1000.ini",
    "FT-1000 MP.ini",
    "FT-100 D.ini",
    "FT-2000.ini",
    "FT-450.ini",
    "FT-747.ini",
    "FT-817.ini",
    "FT-840.ini",
    "FT-847.ini",
    "FT-857.ini",
    "FT-897.ini",
    "FT-900.ini",
    "FT-9000v1.ini",
    "FT-920.ini",
    "FT-950.ini",
    "FT-990.ini",
    "FT-991.ini",
    "FTdx3000.ini",
    "FTDX-5000MP.ini",
    "IC-7000.ini",
    "IC-7100.ini",
    "IC-7200.ini",
    "IC-7300.ini",
    "IC-7410.ini",
    "IC-746.ini",
    "IC-746 Pro.ini",
    "IC-756.ini",
    "IC-756 Pro.ini",
    "IC-756 Pro II.ini",
    "IC-756 Pro III.ini",
    "IC-7600.ini",
    "IC-7600v2.ini",
    "IC-7700.ini",
    "IC-7700v2.ini",
    "IC-7800.ini",
    "IC-7800v3.ini",
    "IC-7850.ini",
    "IC-7851.ini",
    "IC-9100.ini",
    "IC-9100v2.ini",
    "TenTec Eagle.ini",
    "TenTec Orion.ini",
    "TenTec Paragon II.ini",
    "TS-2000.ini",
    "TS-440.ini",
    "TS-480.ini",
    "TS-590.ini",
    "TS-690.ini",
    "TS-870.ini",
    "TS-930.ini"
};

OmniRig::OmniRig(QObject *parent) :
  QObject(parent)
{
    qRegisterMetaType<RigAbstract::Mode>("RigAbstract::Mode");
    search();
}

OmniRig& OmniRig::instance()
{
    static OmniRig INSTANCE;
    return INSTANCE;
}

QStringList OmniRig::protocols() const noexcept
{
    return m_protocols;
}

void OmniRig::search()
{
    for (auto &file : Devices) {
        m_files.append(":/panorama-k/rigs/" + file);
        QFileInfo info(m_files.last());
        assert(info.exists());
        m_protocols << info.baseName();
    }
}

std::unique_ptr<RigAbstract> OmniRig::createRig(QSerialPort &serialPort, const QString &protocol) const
{
    if (m_protocols.contains(protocol))
        return std::make_unique<Rig>(serialPort, m_files.at(m_protocols.indexOf(protocol)));
    return {};
}


}  // namespace PanoramaK
