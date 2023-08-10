// ============================================================================
// author: nightwing
// date  : 2019/05/24
// to do : check point
// ============================================================================

#ifndef QXPACK_IC_DATATRANS_RSHDEBASE_CXX
#define QXPACK_IC_DATATRANS_RSHDEBASE_CXX

#include "qxpack_ic_datatrans_rshdebase.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// inner structure
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcDataTransRsHdeBasePriv {
public:
    IcDataTransFactory m_fact; void *m_fact_ctxt;
    IcDataTransPkgReadBuff   *m_pkg_read_buff;
    IcDataTransPkgWriteBuff  *m_pkg_write_buff;
};

// ////////////////////////////////////////////////////////////////////////////
// implement code
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcDataTransRsHdeBase :: IcDataTransRsHdeBase(
    IcDataTransFactory fact, void *fact_ctxt, QObject *pa
) : QObject(pa)
{
    m_base_priv = new IcDataTransRsHdeBasePriv;
    m_base_priv->m_fact           = fact;
    m_base_priv->m_fact_ctxt      = fact_ctxt;
    m_base_priv->m_pkg_read_buff  = Q_NULLPTR;
    m_base_priv->m_pkg_write_buff = Q_NULLPTR;

    // ------------------------------------------------------------------------
    // make default data reader and writer factory if factory is not set.
    // ------------------------------------------------------------------------
    if ( m_base_priv->m_fact == Q_NULLPTR ) {
        m_base_priv->m_fact = [](void*,const QString& opr, const QVariant& io_dev )-> QVariant {
            if ( opr == STR_IcDataTransFactory_createReader ) {
                QSharedPointer<QIODevice> io_dev_sp = io_dev.value<QSharedPointer<QIODevice>>( );
                return QVariant::fromValue( qobject_cast<QObject*>( new IcDataTransPkgReadBuff ( io_dev_sp )));
            } else if ( opr == STR_IcDataTransFactory_createWriter ) {
                QSharedPointer<QIODevice> io_dev_sp = io_dev.value<QSharedPointer<QIODevice>>( );
                return QVariant::fromValue( qobject_cast<QObject*>( new IcDataTransPkgWriteBuff( io_dev_sp )));
            } else {
                return QVariant();
            }
        };
    }
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransRsHdeBase :: ~IcDataTransRsHdeBase( )
{
    this->deleteBuffer();
    delete m_base_priv;
}

// ============================================================================
// return read buffer pointer
// ============================================================================
IcDataTransPkgReadBuff*  IcDataTransRsHdeBase :: pkgReadBuffer ( )
{   return m_base_priv->m_pkg_read_buff; }

// ============================================================================
// return write buffer pointer
// ============================================================================
IcDataTransPkgWriteBuff* IcDataTransRsHdeBase :: pkgWriteBuffer( )
{  return m_base_priv->m_pkg_write_buff; }

// ============================================================================
// create the buffer
// ============================================================================
void  IcDataTransRsHdeBase :: createBuffer( QSharedPointer<QIODevice> &io_dev )
{
    if ( io_dev.data() == Q_NULLPTR ) { return; }

    // ------------------------------------------------------------------------
    // create read buffer and write buffer
    // ------------------------------------------------------------------------
    QVariant v_rd = m_base_priv->m_fact(
        m_base_priv->m_fact_ctxt, STR_IcDataTransFactory_createReader, QVariant::fromValue( io_dev )
    );
    QVariant v_wt = m_base_priv->m_fact(
        m_base_priv->m_fact_ctxt, STR_IcDataTransFactory_createWriter, QVariant::fromValue( io_dev )
    );
    QObject* rd = v_rd.value<QObject*>();
    QObject* wt = v_wt.value<QObject*>();
    m_base_priv->m_pkg_read_buff = qobject_cast<IcDataTransPkgReadBuff*> ( rd );
    m_base_priv->m_pkg_write_buff= qobject_cast<IcDataTransPkgWriteBuff*>( wt );

    if ( rd != Q_NULLPTR && m_base_priv->m_pkg_read_buff == Q_NULLPTR ) {
        rd->deleteLater();
    }
    if ( wt != Q_NULLPTR && m_base_priv->m_pkg_write_buff == Q_NULLPTR ) {
        wt->deleteLater();
    }
    if ( m_base_priv->m_pkg_read_buff == Q_NULLPTR ) {
        m_base_priv->m_pkg_read_buff  = qxpack_ic_new_qobj( IcDataTransPkgReadBuff, io_dev );
    }
    if ( m_base_priv->m_pkg_write_buff == Q_NULLPTR ) {
        m_base_priv->m_pkg_write_buff = qxpack_ic_new_qobj( IcDataTransPkgWriteBuff, io_dev );
    }

    // ------------------------------------------------------------------------
    // setup the package signal hander
    // ------------------------------------------------------------------------
    QObject::connect(
        m_base_priv->m_pkg_read_buff, SIGNAL(newPackage( const QByteArray& )),
        this, SLOT(readBuff_onNewPackage( const QByteArray& ))
    );
    QObject::connect(
        m_base_priv->m_pkg_write_buff, SIGNAL(packageSent()), this, SLOT(writeBuff_onPackageSent())
    );
}

// ============================================================================
// delete the buffer
// ============================================================================
void   IcDataTransRsHdeBase :: deleteBuffer( )
{
    if ( m_base_priv->m_pkg_read_buff != Q_NULLPTR ) {
        QObject::disconnect( m_base_priv->m_pkg_read_buff, Q_NULLPTR, this, Q_NULLPTR );
        m_base_priv->m_pkg_read_buff->deleteLater();
        m_base_priv->m_pkg_read_buff = Q_NULLPTR;
    }
    if ( m_base_priv->m_pkg_write_buff != Q_NULLPTR ) {
        QObject::disconnect( m_base_priv->m_pkg_write_buff, Q_NULLPTR, this, Q_NULLPTR );
        m_base_priv->m_pkg_write_buff->deleteLater();
        m_base_priv->m_pkg_write_buff = Q_NULLPTR;
    }
}

// ============================================================================
// default readbuffer & write buffer slot
// ============================================================================
void   IcDataTransRsHdeBase :: readBuff_onNewPackage  ( const QByteArray &ba )
{ emit this->newData( ba ); }

void   IcDataTransRsHdeBase :: writeBuff_onPackageSent( )
{ emit this->dataSent(); }

// ============================================================================
// the rs type of this object
// ============================================================================
QString  IcDataTransRsHdeBase :: rsType( ) const { return QString(); }

// ============================================================================
// check if this object is working or not
// ============================================================================
bool     IcDataTransRsHdeBase :: isWorking( ) const { return false; }

// ============================================================================
// open the target
// ============================================================================
void     IcDataTransRsHdeBase :: open (
  const QString &host_name, quint16 port, int om
) {
    Q_UNUSED( host_name ); Q_UNUSED( port ); Q_UNUSED( om );
}

// ============================================================================
// initailize by descriptor
// ============================================================================
bool     IcDataTransRsHdeBase :: initByDscr ( QVariant d )
{   Q_UNUSED( d ); return false;  }

// ============================================================================
// close current device
// ============================================================================
void     IcDataTransRsHdeBase :: close( ) { }

// ============================================================================
// close current device
// ============================================================================
bool     IcDataTransRsHdeBase :: post ( const QByteArray &ba, bool f )
{ Q_UNUSED( ba ); Q_UNUSED( f ); return false; }

// ============================================================================
// full host name
// ============================================================================
QString  IcDataTransRsHdeBase :: fullHostName() const
{ return QString(); }

}


// ////////////////////////////////////////////////////////////////////////////
//
//                 static factory
//
// ////////////////////////////////////////////////////////////////////////////
#include "qxpack/indcom/net/qxpack_ic_datatrans_rshde_impl_bundle.hxx"

namespace QxPack {

// ============================================================================
// builder function
// ============================================================================
IcDataTransRsHdeBase *   IcDataTransRsHdeBase :: createObj (
    const QString &rs_type, IcDataTransFactory fact, void *ctxt, QObject *pa
) {
    if        ( rs_type == IcDataTransRsHdeLoc::rsTypeName() ) {
        return qxpack_ic_new_qobj( IcDataTransRsHdeLoc, fact, ctxt, pa );
    } else if ( rs_type == IcDataTransRsHdeTcp::rsTypeName() ) {
        return qxpack_ic_new_qobj( IcDataTransRsHdeTcp, fact, ctxt, pa );
    } else {
        return Q_NULLPTR;
    }
}

// ============================================================================
// destroy function
// ============================================================================
void  IcDataTransRsHdeBase :: deleteObj( IcDataTransRsHdeBase *rs )
{
    if ( rs != Q_NULLPTR ) { rs->close(); rs->deleteLater(); }
}

// ============================================================================
// return all avaiable hde list
// ============================================================================
QStringList  IcDataTransRsHdeBase :: avaliableRsTypeList()
{
    QStringList list;
    list << IcDataTransRsHdeLoc::rsTypeName() << IcDataTransRsHdeTcp::rsTypeName();
    return list;
}


}

#endif
