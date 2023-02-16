#ifndef USBDEV_FRAMEDATA_HXX
#define USBDEV_FRAMEDATA_HXX

#include "usbdev/common/usbdev_def.h"
#include <QByteArray>
#include <QMetaType>
#include <QImage>
#include <QSize>

namespace UsbDev {

class Profile;

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief the front video frame data
 */
// ////////////////////////////////////////////////////////////////////////////
class USBDEV_API  FrameData {
public:
    //! ctor
    explicit FrameData ( );

    //! ctor by raw data
    //! @param ba [in] the raw data
    //! @param pf [in] the profile that spec. video width, height and format...
    //! @note this CTOR is called inside DevCtl, user should not use it.
    FrameData ( const QByteArray &ba);

    //! ctor ( construct )
    FrameData ( const FrameData & );

    //! assign
    FrameData & operator = ( const FrameData & );

    //! dtor
    virtual   ~FrameData( );

    //! check if it is empty
    bool        isEmpty( ) const;

    quint32&      crc_veryfication( ) const;

    //! return the time stamp of this frame data
    //! @note the empty FrameData will return 0
    quint32&      timeStamp( ) const;

    bool& shutterStatus() const;
    quint16& stimulateDotSerialNumber() const;
    qint32& xMotorCoordinate() const;
    qint32& yMotorCoordinate() const;

    //! return the raw data
    //! @return if OK, return object is not empty
    //! @note user should check return object whether if it is empty.
    //! @note the empty FrameData will return empty QByteArray()
    QByteArray&  rawData( ) const;

private:
    void *m_obj;
};

}

Q_DECLARE_METATYPE(UsbDev::FrameData)

#endif
