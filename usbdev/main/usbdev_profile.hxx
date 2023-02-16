// ////////////////////////////////////////////////////////////////////////////
/*!
 * @verbatim
 *     < author >      < date >         < memo. >
 *      nightwing      2019/06/19       fixed. by new instruction of doc.
 *      nightwing      2020/10/13       added new functions()  for 0.2.0
 *      nightwing      2020/12/15       added physical key count for 0.3.0
 * @endverbatim
 */
// ////////////////////////////////////////////////////////////////////////////

#ifndef USBDEV_PROFILE_HXX
#define USBDEV_PROFILE_HXX

#include "usbdev/common/usbdev_def.h"
#include "usbdev/main/usbdev_devctl.hxx"
#include <QString>
#include <QSize>
#include <QPair>
#include <QByteArray>
#include <QMetaType>
#include <QJsonObject>

namespace UsbDev {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief describe the hardware capabilities
 */
// ////////////////////////////////////////////////////////////////////////////
class USBDEV_API  Profile {
public:
   //! ctor
   explicit Profile ( );

   //! construct by spec. data
   Profile ( const QByteArray & );

   //! ctor ( construct )
   Profile ( const Profile & );

   //! ctor ( assign )
   Profile & operator = ( const Profile & );

   //! dtor
   virtual ~Profile( );

   //! check if it is empty
   bool     isEmpty( ) const;

   quint16 devType() const;

   quint16 devVersion() const;

   QSize videoSize() const;

   //! return the range of the chinresets
   QPair<qint32,qint32> motorRange( DevCtl::MotorId ) const;

//   //! to json object
//   QJsonObject  toJsonObject( ) const;

private:
    void *m_obj;
};



}

Q_DECLARE_METATYPE(UsbDev::Profile)
#endif
