#ifndef QXPACK_IC_USBSTORMON_CXX
#define QXPACK_IC_USBSTORMON_CXX

#include "qxpack_ic_usbstormon.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// register in QT meta system
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_reg = false;
static void gRegInQt ( )
{
    if ( ! g_is_reg ) {
        qRegisterMetaType<QxPack::IcUsbStorInfo>(); g_is_reg = true;
        qRegisterMetaType<QList<QxPack::IcUsbStorInfo>>(); g_is_reg = true;
    }
}

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcUsbStorInfoPriv*, o )
class QXPACK_IC_HIDDEN IcUsbStorInfoPriv : public IcPImplPrivTemp<IcUsbStorInfoPriv> {
private:
    QString  m_root_path, m_label;
    quint64  m_cap_size,  m_freespc_size;
public :
    explicit IcUsbStorInfoPriv( );
    IcUsbStorInfoPriv ( const IcUsbStorInfoPriv &ot );
    virtual ~IcUsbStorInfoPriv( ) Q_DECL_OVERRIDE { }
    inline QString & rootPathRef() { return m_root_path; }
    inline QString & labelRef()    { return m_label; }
    inline quint64 & capSizeRef()  { return m_cap_size; }
    inline quint64 & freeSpcRef()  { return m_freespc_size; }
};

IcUsbStorInfoPriv :: IcUsbStorInfoPriv ( )
{
    m_cap_size = 0; m_freespc_size = 0;
}

IcUsbStorInfoPriv :: IcUsbStorInfoPriv ( const IcUsbStorInfoPriv &ot )
{
    m_root_path = ot.m_root_path; m_label = ot.m_label;
    m_cap_size  = ot.m_cap_size;  m_freespc_size = ot.m_freespc_size;
}

IcUsbStorInfo :: IcUsbStorInfo ( )
{ gRegInQt(); m_obj = nullptr; }

IcUsbStorInfo :: ~IcUsbStorInfo ( )
{ makeNull(); }

IcUsbStorInfo :: IcUsbStorInfo ( const IcUsbStorInfo &ot )
{
    gRegInQt(); m_obj = nullptr;
    IcUsbStorInfoPriv::attach( &m_obj, const_cast<void**>( &ot.m_obj ));
}

IcUsbStorInfo &   IcUsbStorInfo :: operator = ( const IcUsbStorInfo &ot )
{
    gRegInQt();
    IcUsbStorInfoPriv::attach( &m_obj, const_cast<void**>( &ot.m_obj ));
    return *this;
}

bool               IcUsbStorInfo :: isNull() const
{ return ( m_obj == nullptr ); }

void               IcUsbStorInfo :: makeNull()
{
    if ( m_obj != nullptr ) {
        IcUsbStorInfoPriv::attach( &m_obj, nullptr );
    }
}

QString            IcUsbStorInfo :: rootPath() const
{ return ( m_obj != nullptr ? T_PrivPtr(m_obj)->rootPathRef() : QString()); }

void               IcUsbStorInfo :: setRootPath( const QString &rp )
{ IcUsbStorInfoPriv::instanceCow(&m_obj)->rootPathRef() = rp; }

QString            IcUsbStorInfo :: label() const
{ return ( m_obj != nullptr ? T_PrivPtr(m_obj)->labelRef() : QString()); }

void               IcUsbStorInfo :: setLabel( const QString &lbl )
{ IcUsbStorInfoPriv::instanceCow(&m_obj)->labelRef() = lbl; }

quint64            IcUsbStorInfo :: capacity() const
{ return ( m_obj != nullptr ? T_PrivPtr(m_obj)->capSizeRef() : 0 ); }

void               IcUsbStorInfo :: setCapacity( const quint64 &c )
{ IcUsbStorInfoPriv::instanceCow(&m_obj)->capSizeRef() = c; }

quint64            IcUsbStorInfo :: freeSpace() const
{ return ( m_obj != nullptr ? T_PrivPtr(m_obj)->freeSpcRef() : 0 ); }

void               IcUsbStorInfo :: setFreeSpace( const quint64 &s )
{ IcUsbStorInfoPriv::instanceCow(&m_obj)->freeSpcRef() = s; }

}

#endif
