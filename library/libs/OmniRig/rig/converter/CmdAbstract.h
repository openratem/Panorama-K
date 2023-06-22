#pragma once

#include <QByteArray>

class CmdAbstract
{
public:
    /**
     *
     */
    typedef enum
    {
        pmNone  = -1,
        pmFreqA = 0,//!< VFO A frequency
        pmFreqB,    //!< VFO B frequency
        pmFreq,     //!< operating frequency
        pmRitOffset,//!< RIT offset frequency
        pmRit0,     //!< Clear RIT - this is a write-only parameter
        pmPitch,    //!< CW pitch frequency
        pmSplitOn,  //!< enable split operation
        pmSplitOff, //!< disable split operation
        pmVfoA,     //!< receive on VFO A, transmit VFO unknown
        pmVfoB,     //!< receive on VFO B, transmit VFO unknown
        pmVfoEqual, //!< copy the frequency of the receive VFO to the transmit VFO
        pmVfoSwap,  //!< swap frequencies of the receive and transmit VFO's
        pmVfoAA,    //!< receive and transmit on VFO A
        pmVfoAB,    //!< receive on VFO A, transmit on VFO B
        pmVfoBA,    //!< receive on VFO B, transmit on VFO A
        pmVfoBB,    //!< receive and transmit on VFO B
        pmRitOn,    //!< enable RIT
        pmRitOff,   //!< disable RIT
        pmXitOn,    //!< enable XIT
        pmXitOff,   //!< disable XIT
        pmRx,       //!< enable receive mode
        pmTx,       //!< enable transmit mode
        pmCW_U,     //!< CW mode, upper sideband
        pmCW_L,     //!< CW mode, lower sideband
        pmSSB_U,    //!< USB mode
        pmSSB_L,    //!< LSB mode
        pmDIG_U,    //!< Digital mode (RTTY, FSK, etc.), upper sideband
        pmDIG_L,    //!< Digital mode, lower sideband
        pmAM,       //!< AM mode
        pmFM,       //!< FM mode

        XmdICount   //!< XmdICount
    }CmdId;

    /**
     *
     * \details These examples show the values of 123 and -123 converted to
     * a sequence of 4 bytes according to different formats:
     *
     * Value:       123          -123
     * ----------------------------------
     * vfText   30.31.32.33   2D.31.32.33
     * vfBinL   7B.00.00.00   85.FF.FF.FF
     * vfBinB   00.00.00.7B   FF.FF.FF.85
     * vfBcdLU  23.01.00.00   n/a
     * vfBcdLS  23.01.00.00   23.01.00.FF
     * vfBcdBU  00.00.01.23   n/a
     * vfBcdBS  00.00.01.23   FF.00.01.23
     * vfYaesu  00.00.00.7B   80.00.00.7B
     */
    typedef enum
    {
        vfNone = -1,
        vfText = 0,//!< asc codes of digits
        vfBinL,    //!< integer, little endian
        vfBinB,    //!< integer, big endian
        vfBcdLU,   //!< BCD, little endian, unsigned
        vfBcdLS,   //!< BCD, little endian, signed; the sign is in the MSB byte (0x00 or 0xFF)
        vfBcdBU,   //!< big endian, unsigned
        vfBcdBS,   //!< big endian, signed
        vfYaesu    //!< special format used by Yaesu
    }NumberFormat;

public:
    CmdAbstract() {}
    virtual ~CmdAbstract() {}

    virtual bool avalible() const { return false; }
    virtual quint32 count() const { return 0u; }
    virtual QByteArray getCmd(qint32 value = 0) { Q_UNUSED(value) return QByteArray(); }
    virtual QByteArray reply() { return QByteArray(); }
    virtual bool isValid(quint32 cmdId, QByteArray &data) { Q_UNUSED(cmdId) Q_UNUSED(data) return false; }
};

