#ifndef JRPCPLAT_MEMCNTR_HXX
#define JRPCPLAT_MEMCNTR_HXX
 
#include "jrpcplat_def.h"

#ifdef JRPCPLAT_QT5_ENABLED
#include <QObject>
#endif


namespace JRpcPlat {

// ////////////////////////////////////////////////////////////////////////////
//
// memory counter
//
// Tips:
// 1) enable memory trace.
//    define the JRPCPLAT_CFG_MEM_TRACE
//    call  MemCntr::enableMemTrace( ) to enable or disable memory trace
//
// 2) use jrpcplat_new() / jrpcplat_delete() to manage the new / delete normal object
//
// 3) use jrpcplat_new_qobj() / jrpcplat_delete_qobj() to manage qobject
//
// ////////////////////////////////////////////////////////////////////////////
class JRPCPLAT_API MemCntr {
public:
    static int    currNewCntr( );
    static void*  _assertNonNullPtr( void* p, bool is_alloc ); // nw: 20180807 added
    static int    incrNewCntr( int cntr, const char *file = nullptr, const char *func = nullptr, int ln = 0, const char *type = nullptr );
    static int    decrNewCntr( int cntr, const char *file = nullptr, const char *func = nullptr, int ln = 0, const char *type = nullptr );
    static void   saveTraceInfoToFile( const char *fn );
    static void   enableMemTrace( bool sw );

    // nw: 2018/11 added
    static void*  allocMemory  ( size_t size );
    static void*  reallocMemory( void *old, size_t size );
    static void   freeMemory ( void *p );

    // nw: 2019/05 added
    static void   enableMemCntr( bool sw ); // default is true

    // nw: 2019/05/23 added for QT5
#if defined( JRPCPLAT_QT5_ENABLED )
    static QObject*  memCntOnQObj  ( QObject*, const char *file, const char *func, int ln, const char *type );
    static void      memCntOnDelObj( QObject *o, bool is_later = false );
#endif

protected:
    MemCntr ( );
    virtual ~MemCntr( );
private:
    void *m_obj;
};

}

#if defined( JRPCPLAT_CFG_MEM_TRACE )

#define  jrpcplat_new( t, ... )       ( JRpcPlat::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ),  static_cast< t *>( JRpcPlat::MemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true )) )
#define  jrpcplat_delete( o, t )      do{ JRpcPlat::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ); delete( static_cast< t *>( JRpcPlat::MemCntr::_assertNonNullPtr( o, false )) ); }while(0)
#define  jrpcplat_incr_new_cntr( t )  JRpcPlat::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )
#define  jrpcplat_decr_new_cntr( t )  JRpcPlat::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )

#define  jrpcplat_alloc( sz )         ( JRpcPlat::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__), JRpcPlat::MemCntr::_assertNonNullPtr( JRpcPlat::MemCntr::allocMemory( sz ), true ))
#define  jrpcplat_realloc( p, sz )    ( JRpcPlat::MemCntr::incrNewCntr(( p != nullptr ? 0 : 1 ),__FILE__,__FUNCTION__,__LINE__), JRpcPlat::MemCntr::_assertNonNullPtr( JRpcPlat::MemCntr::reallocMemory( p, sz ), true ))
#define  jrpcplat_free( p )           do{ JRpcPlat::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__); JRpcPlat::MemCntr::freeMemory( JRpcPlat::MemCntr::_assertNonNullPtr( p, false )); }while(0)


#else

#define  jrpcplat_new( t, ... )       ( JRpcPlat::MemCntr::incrNewCntr(1 ), static_cast< t *>( JRpcPlat::MemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true ) ))
#define  jrpcplat_delete( o, t )      do{ JRpcPlat::MemCntr::decrNewCntr(1); delete( static_cast< t *>( JRpcPlat::MemCntr::_assertNonNullPtr( o, false ) )); }while(0)
#define  jrpcplat_incr_new_cntr( t )  JRpcPlat::MemCntr::incrNewCntr(1)
#define  jrpcplat_decr_new_cntr( t )  JRpcPlat::MemCntr::decrNewCntr(1)

#define  jrpcplat_alloc( sz )         ( JRpcPlat::MemCntr::incrNewCntr(1), JRpcPlat::MemCntr::_assertNonNullPtr( JRpcPlat::MemCntr::allocMemory( sz ), true ))
#define  jrpcplat_realloc( p, sz )    ( JRpcPlat::MemCntr::incrNewCntr(( p != nullptr ? 0 : 1 )), JRpcPlat::MemCntr::_assertNonNullPtr( JRpcPlat::MemCntr::reallocMemory( p, sz ), true ))
#define  jrpcplat_free( p )           do{ JRpcPlat::MemCntr::decrNewCntr(1); JRpcPlat::MemCntr::freeMemory( JRpcPlat::MemCntr::_assertNonNullPtr( p, false )); }while(0)

#endif

#define  jrpcplat_curr_new_cntr           ( JRpcPlat::MemCntr::currNewCntr())
#define  jrpcplat_enable_mem_trace( sw )  JRpcPlat::MemCntr::enableMemTrace( sw )
#define  jrpcplat_enable_mem_cntr( sw )   JRpcPlat::MemCntr::enableMemCntr( sw )



#ifdef JRPCPLAT_QT5_ENABLED
// ////////////////////////////////////////////////////////////////////////////
//
//             added for QT5
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
/*!
 * @brief memory counted new QObject
 * @details user can use this macro to do count on classes inherit QObject
 * @warning This mechanism used QObject::destroyed signal, do not do disconnect like \n
 *     obj::disconnect() or QObject::disconnect( obj, 0,0,0 ) \n
 *     that will discard signals. If possible, use obj::blockSignals(true) to instead
 *     block emit signals.
 * @note  For third library object, maybe inner discard all signals, so user can \n
 *     manual free it use jrpcplat_delete_qobj( o )
 */
// ============================================================================
#define jrpcplat_new_qobj( t, ... ) \
    ( \
     [=]( t* _t, const char *fn, const char *func, int ln, const char *type )-> t * { \
         return qobject_cast< t *>( JRpcPlat::MemCntr::memCntOnQObj( _t, fn, func, ln, type )); \
     } \
    )( new t ( __VA_ARGS__ ), __FILE__, __FUNCTION__, __LINE__, #t )



#define jrpcplat_delete_qobj( o ) \
    do{ JRpcPlat::MemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), false ); } while(0)

#define jrpcplat_delete_qobj_later( o ) \
    do{ JRpcPlat::MemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), true ); } while(0)

#endif




#endif
