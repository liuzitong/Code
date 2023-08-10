#ifndef QXPACK_IC_USBSTORMON_HXX
#define QXPACK_IC_USBSTORMON_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>


namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//  the usb storage information
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcUsbStorInfo QXPACK_IC_FINAL {
public:
    //! ctor
    explicit IcUsbStorInfo( );

    ~IcUsbStorInfo( );

    IcUsbStorInfo ( const IcUsbStorInfo & );

    IcUsbStorInfo &  operator = ( const IcUsbStorInfo & );

#ifdef QXPACK_IC_CXX11
    IcUsbStorInfo ( IcUsbStorInfo &&ot ) { m_obj = ot.m_obj; ot.m_obj = nullptr; }
    IcUsbStorInfo &  operator = ( IcUsbStorInfo &&ot )
    { if ( this != &ot  &&  m_obj != ot.m_obj ) { if ( m_obj != nullptr ){ makeNull(); } m_obj = ot.m_obj; ot.m_obj = nullptr; } return *this; }
#endif

    bool  isNull() const;

    void  makeNull();

    //! access the root path of this usb storage
    QString   rootPath() const;
    void      setRootPath( const QString & );

    //! access the label name of this usb storage
    QString   label() const;
    void      setLabel( const QString & );

    //! access the total capacity in bytes
    quint64   capacity() const;
    void      setCapacity( const quint64 & );

    //! access the free space in bytes
    quint64   freeSpace() const;
    void      setFreeSpace( const quint64 & );

private:
    void *m_obj;
};



// ////////////////////////////////////////////////////////////////////////////
//
// the usb storage monitor
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcUsbStorMon : public QObject {
    Q_OBJECT
public:
    //! ctor
    explicit IcUsbStorMon( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcUsbStorMon() Q_DECL_OVERRIDE;

    //! get information list
    QList<QxPack::IcUsbStorInfo>  infoList( ) const;

    //! try eject spec USB stor volume
    //! @return true for ejected
    bool         ejectVol  ( const QString &vol_path );

    Q_SIGNAL void  listChanged( );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcUsbStorMon )
};
}

Q_DECLARE_METATYPE(QxPack::IcUsbStorInfo)
Q_DECLARE_METATYPE(QList<QxPack::IcUsbStorInfo>)
#endif
