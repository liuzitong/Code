#ifndef QXPACK_IC_USBSTORMON_NULL_CXX
#define QXPACK_IC_USBSTORMON_NULL_CXX

#include "qxpack_ic_usbstormon.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// empty implement
// ////////////////////////////////////////////////////////////////////////////
IcUsbStorMon :: IcUsbStorMon ( QObject *pa ) : QObject( pa )
{ m_obj = Q_NULLPTR; }

IcUsbStorMon :: ~IcUsbStorMon( )
{  }

QStringList  IcUsbStorMon :: volPathList( ) const
{ return QStringList(); }

QStringList  IcUsbStorMon :: volLabelList() const
{ return QStringList(); }

bool         IcUsbStorMon :: ejectVol  ( const QString &vol_path )
{ Q_UNUSED(vol_path); return false; }


}

#endif
