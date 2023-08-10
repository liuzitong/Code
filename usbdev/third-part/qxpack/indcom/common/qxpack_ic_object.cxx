#ifndef QXPACK_IC_OBJECT_CXX
#define QXPACK_IC_OBJECT_CXX

#include "qxpack_ic_object.hxx"
#include "qxpack_ic_memcntr.hxx"

#include <thread> //C++11

#ifndef  QXPACK_IC_CXX11
# error "require C++11 !"
#endif

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//                     remove all events by object pointer
// ////////////////////////////////////////////////////////////////////////////
namespace IcEvtSysSpc {
//QXPACK_IC_HIDDEN void  rmvAllEventByObj( IcObject* );
}

// ////////////////////////////////////////////////////////////////////////////
//                      thread local variable
// ////////////////////////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////
//                      object private information
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcObjectPriv*, o )
class QXPACK_IC_HIDDEN  IcObjectPriv {
private:

public :
    explicit IcObjectPriv( void * );
    ~IcObjectPriv( ) { }
};

// ============================================================================
// ctor
// ============================================================================
IcObjectPriv :: IcObjectPriv ( void* )
{

}


// ////////////////////////////////////////////////////////////////////////////
//                      API wrapper
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcObject :: IcObject ( void *rsvd )
{
    m_obj = qxpack_ic_new( IcObjectPriv, rsvd );
}

// ============================================================================
// dtor
// ============================================================================
IcObject :: ~IcObject( )
{
   // IcEvtSysSpc::rmvAllEventByObj( this );
    qxpack_ic_delete( m_obj, IcObjectPriv );
}

// ============================================================================
// event handler
// ============================================================================
bool                    IcObject :: event( IcEvent *e )
{ QXPACK_IC_UNUSED(e); return false; }



}
#endif
