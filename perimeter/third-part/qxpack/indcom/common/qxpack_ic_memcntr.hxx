#ifndef QXPACK_IC_MEMCNTR_HXX
#define QXPACK_IC_MEMCNTR_HXX
 
#include "qxpack/indcom/common/qxpack_ic_def.h"

#ifdef QXPACK_IC_QT_ENABLED
#include <QObject>
#endif


namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// memory counter
//
// Tips:
// 1) enable memory trace.
//    define the QXPACK_IC_CFG_MEM_TRACE
//    call  IcMemCntr::enableMemTrace( ) to enable or disable memory trace
//
// 2) use qxpack_ic_new() / qxpack_ic_delete() to manage the new / delete normal object
//
// 3) use qxpack_ic_new_qobj() / qxpack_ic_delete_qobj() to manage qobject
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcMemCntr QXPACK_IC_FINAL {
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
#if defined( QXPACK_IC_QT_ENABLED )
    static QObject*  memCntOnQObj  ( QObject*, const char *file, const char *func, int ln, const char *type );
    static void      memCntOnDelObj( QObject *o, bool is_later = false );
#endif

protected:
    IcMemCntr ( );
    ~IcMemCntr( );
private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcMemCntr )
};

}

#if defined( QXPACK_IC_CFG_MEM_TRACE )

#define  qxpack_ic_new( t, ... )       ( QxPack::IcMemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ),  static_cast< t *>( QxPack::IcMemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true )) )
#define  qxpack_ic_delete( o, t )      do{ QxPack::IcMemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ); delete( static_cast< t *>( QxPack::IcMemCntr::_assertNonNullPtr( o, false )) ); }while(0)
#define  qxpack_ic_incr_new_cntr( t )  QxPack::IcMemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )
#define  qxpack_ic_decr_new_cntr( t )  QxPack::IcMemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )

#define  qxpack_ic_alloc( sz )         ( QxPack::IcMemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__), QxPack::IcMemCntr::_assertNonNullPtr( QxPack::IcMemCntr::allocMemory( sz ), true ))
#define  qxpack_ic_realloc( p, sz )    ( QxPack::IcMemCntr::incrNewCntr(( p != nullptr ? 0 : 1 ),__FILE__,__FUNCTION__,__LINE__), QxPack::IcMemCntr::_assertNonNullPtr( QxPack::IcMemCntr::reallocMemory( p, sz ), true ))
#define  qxpack_ic_free( p )           do{ QxPack::IcMemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__); QxPack::IcMemCntr::freeMemory( QxPack::IcMemCntr::_assertNonNullPtr( p, false )); }while(0)


#else

#define  qxpack_ic_new( t, ... )       ( QxPack::IcMemCntr::incrNewCntr(1 ), static_cast< t *>( QxPack::IcMemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true ) ))
#define  qxpack_ic_delete( o, t )      do{ QxPack::IcMemCntr::decrNewCntr(1); delete( static_cast< t *>( QxPack::IcMemCntr::_assertNonNullPtr( o, false ) )); }while(0)
#define  qxpack_ic_incr_new_cntr( t )  QxPack::IcMemCntr::incrNewCntr(1)
#define  qxpack_ic_decr_new_cntr( t )  QxPack::IcMemCntr::decrNewCntr(1)

#define  qxpack_ic_alloc( sz )         ( QxPack::IcMemCntr::incrNewCntr(1), QxPack::IcMemCntr::_assertNonNullPtr( QxPack::IcMemCntr::allocMemory( sz ), true ))
#define  qxpack_ic_realloc( p, sz )    ( QxPack::IcMemCntr::incrNewCntr(( p != nullptr ? 0 : 1 )), QxPack::IcMemCntr::_assertNonNullPtr( QxPack::IcMemCntr::reallocMemory( p, sz ), true ))
#define  qxpack_ic_free( p )           do{ QxPack::IcMemCntr::decrNewCntr(1); QxPack::IcMemCntr::freeMemory( QxPack::IcMemCntr::_assertNonNullPtr( p, false )); }while(0)

#endif

#define  qxpack_ic_curr_new_cntr           ( QxPack::IcMemCntr::currNewCntr())
#define  qxpack_ic_enable_mem_trace( sw )  QxPack::IcMemCntr::enableMemTrace( sw )
#define  qxpack_ic_enable_mem_cntr( sw )   QxPack::IcMemCntr::enableMemCntr( sw )



#ifdef QXPACK_IC_QT_ENABLED
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
 *     manual free it use qxpack_ic_delete_qobj( o )
 */
// ============================================================================
#define qxpack_ic_new_qobj( t, ... ) \
    ( \
     [=]( t* _t, const char *fn, const char *func, int ln, const char *type )-> t * { \
         return qobject_cast< t *>( QxPack::IcMemCntr::memCntOnQObj( _t, fn, func, ln, type )); \
     } \
    )( new t ( __VA_ARGS__ ), __FILE__, __FUNCTION__, __LINE__, #t )



#define qxpack_ic_delete_qobj( o ) \
    do{ QxPack::IcMemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), false ); } while(0)

#define qxpack_ic_delete_qobj_later( o ) \
    do{ QxPack::IcMemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), true ); } while(0)

#endif




#endif
