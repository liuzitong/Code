// ============================================================================
// author: nightwing
// date  : 2019/05/24
// to do : check point
// ============================================================================
#ifndef QXPACK_IC_DATATRANS_RSSVRBASE_CXX
#define QXPACK_IC_DATATRANS_RSSVRBASE_CXX

#include "qxpack_ic_datatrans_rssvrbase.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// implement code
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcDataTransRsSvrBase :: IcDataTransRsSvrBase( QObject *pa ) : QObject(pa)
{
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransRsSvrBase :: ~IcDataTransRsSvrBase( )
{
}

// ============================================================================
// start the server
// ============================================================================
bool   IcDataTransRsSvrBase :: startServer( const QString &, quint16 ) { return false; }

// ============================================================================
// stop the server
// ============================================================================
void   IcDataTransRsSvrBase :: stopServer() { }

// ============================================================================
// check if listening
// ============================================================================
bool   IcDataTransRsSvrBase :: isListening( ) const { return false; }

// ============================================================================
// return null string
// ============================================================================
QString  IcDataTransRsSvrBase :: rsType( ) const { return QString(); }

// ============================================================================
// the port of server ( if existed )
// ============================================================================
quint16  IcDataTransRsSvrBase :: port( ) const { return 0; }

// ============================================================================
// return the full server name of this server
// ============================================================================
QString  IcDataTransRsSvrBase :: fullServerName( ) const { return QString(); }

}


// ////////////////////////////////////////////////////////////////////////////
//
//                 static factory
//
// ////////////////////////////////////////////////////////////////////////////
#include "qxpack/indcom/net/qxpack_ic_datatrans_rssvr_impl_bundle.hxx"

namespace QxPack {

// ============================================================================
// builder function
// ============================================================================
IcDataTransRsSvrBase *   IcDataTransRsSvrBase :: createObj (
    const QString &rs_type, QObject *pa
) {
    if ( rs_type == IcDataTransRsSvrLoc::rsTypeName() ) {
        return qxpack_ic_new_qobj( IcDataTransRsSvrLoc, pa );
    } else if ( rs_type == IcDataTransRsSvrTcp::rsTypeName()) {
        return qxpack_ic_new_qobj( IcDataTransRsSvrTcp, pa );
    } else {
        return Q_NULLPTR;
    }
}

// ============================================================================
// destroy function
// ============================================================================
void  IcDataTransRsSvrBase :: deleteObj( IcDataTransRsSvrBase *rs )
{
    if ( rs != Q_NULLPTR ) { rs->stopServer(); rs->deleteLater(); }
}

// ============================================================================
// return all avaiable hde list
// ============================================================================
QStringList  IcDataTransRsSvrBase :: avaliableRsTypeList()
{
    QStringList list;
    list << IcDataTransRsSvrLoc::rsTypeName() << IcDataTransRsSvrTcp::rsTypeName();
    return list;
}


}

#endif
