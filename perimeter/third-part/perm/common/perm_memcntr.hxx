#ifndef PERM_MEMCNTR_HXX
#define PERM_MEMCNTR_HXX
 
#include "perm_def.h"

#ifdef PERM_QT5_ENABLED
#include <QObject>
#endif


namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//
// memory counter
//
// Tips:
// 1) enable memory trace.
//    define the PERM_CFG_MEM_TRACE
//    call  MemCntr::enableMemTrace( ) to enable or disable memory trace
//
// 2) use perm_new() / perm_delete() to manage the new / delete normal object
//
// 3) use perm_new_qobj() / perm_delete_qobj() to manage qobject
//
// ////////////////////////////////////////////////////////////////////////////
class PERM_API MemCntr {
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
#if defined( PERM_QT5_ENABLED )
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

#if defined( PERM_CFG_MEM_TRACE )

#define  perm_new( t, ... )       ( FcPerm::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ),  static_cast< t *>( FcPerm::MemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true )) )
#define  perm_delete( o, t )      do{ FcPerm::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ); delete( static_cast< t *>( FcPerm::MemCntr::_assertNonNullPtr( o, false )) ); }while(0)
#define  perm_incr_new_cntr( t )  FcPerm::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )
#define  perm_decr_new_cntr( t )  FcPerm::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )

#define  perm_alloc( sz )         ( FcPerm::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__), FcPerm::MemCntr::_assertNonNullPtr( FcPerm::MemCntr::allocMemory( sz ), true ))
#define  perm_realloc( p, sz )    ( FcPerm::MemCntr::incrNewCntr(( p != nullptr ? 0 : 1 ),__FILE__,__FUNCTION__,__LINE__), FcPerm::MemCntr::_assertNonNullPtr( FcPerm::MemCntr::reallocMemory( p, sz ), true ))
#define  perm_free( p )           do{ FcPerm::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__); FcPerm::MemCntr::freeMemory( FcPerm::MemCntr::_assertNonNullPtr( p, false )); }while(0)


#else

#define  perm_new( t, ... )       ( FcPerm::MemCntr::incrNewCntr(1 ), static_cast< t *>( FcPerm::MemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true ) ))
#define  perm_delete( o, t )      do{ FcPerm::MemCntr::decrNewCntr(1); delete( static_cast< t *>( FcPerm::MemCntr::_assertNonNullPtr( o, false ) )); }while(0)
#define  perm_incr_new_cntr( t )  FcPerm::MemCntr::incrNewCntr(1)
#define  perm_decr_new_cntr( t )  FcPerm::MemCntr::decrNewCntr(1)

#define  perm_alloc( sz )         ( FcPerm::MemCntr::incrNewCntr(1), FcPerm::MemCntr::_assertNonNullPtr( FcPerm::MemCntr::allocMemory( sz ), true ))
#define  perm_realloc( p, sz )    ( FcPerm::MemCntr::incrNewCntr(( p != nullptr ? 0 : 1 )), FcPerm::MemCntr::_assertNonNullPtr( FcPerm::MemCntr::reallocMemory( p, sz ), true ))
#define  perm_free( p )           do{ FcPerm::MemCntr::decrNewCntr(1); FcPerm::MemCntr::freeMemory( FcPerm::MemCntr::_assertNonNullPtr( p, false )); }while(0)

#endif

#define  perm_curr_new_cntr           ( FcPerm::MemCntr::currNewCntr())
#define  perm_enable_mem_trace( sw )  FcPerm::MemCntr::enableMemTrace( sw )
#define  perm_enable_mem_cntr( sw )   FcPerm::MemCntr::enableMemCntr( sw )



#ifdef PERM_QT5_ENABLED
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
 *     manual free it use perm_delete_qobj( o )
 */
// ============================================================================
#define perm_new_qobj( t, ... ) \
    ( \
     [=]( t* _t, const char *fn, const char *func, int ln, const char *type )-> t * { \
         return qobject_cast< t *>( FcPerm::MemCntr::memCntOnQObj( _t, fn, func, ln, type )); \
     } \
    )( new t ( __VA_ARGS__ ), __FILE__, __FUNCTION__, __LINE__, #t )



#define perm_delete_qobj( o ) \
    do{ FcPerm::MemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), false ); } while(0)

#define perm_delete_qobj_later( o ) \
    do{ FcPerm::MemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), true ); } while(0)

#endif




#endif
