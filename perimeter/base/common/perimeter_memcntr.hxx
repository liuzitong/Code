#ifndef PERIMETER_MEMCNTR_HXX
#define PERIMETER_MEMCNTR_HXX
 
#include "perimeter_def.h"

#ifdef PERIMETER_QT5_ENABLED
#include <QObject>
#endif


namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
//
// memory counter
//
// Tips:
// 1) enable memory trace.
//    define the PERIMETER_CFG_MEM_TRACE
//    call  MemCntr::enableMemTrace( ) to enable or disable memory trace
//
// 2) use perimeter_new() / perimeter_delete() to manage the new / delete normal object
//
// 3) use perimeter_new_qobj() / perimeter_delete_qobj() to manage qobject
//
// ////////////////////////////////////////////////////////////////////////////
class PERIMETER_API MemCntr {
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
#if defined( PERIMETER_QT5_ENABLED )
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

#if defined( PERIMETER_CFG_MEM_TRACE )

#define  perimeter_new( t, ... )       ( Perimeter::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ),  static_cast< t *>( Perimeter::MemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true )) )
#define  perimeter_delete( o, t )      do{ Perimeter::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ); delete( static_cast< t *>( Perimeter::MemCntr::_assertNonNullPtr( o, false )) ); }while(0)
#define  perimeter_incr_new_cntr( t )  Perimeter::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )
#define  perimeter_decr_new_cntr( t )  Perimeter::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )

#define  perimeter_alloc( sz )         ( Perimeter::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__), Perimeter::MemCntr::_assertNonNullPtr( Perimeter::MemCntr::allocMemory( sz ), true ))
#define  perimeter_realloc( p, sz )    ( Perimeter::MemCntr::incrNewCntr(( p != nullptr ? 0 : 1 ),__FILE__,__FUNCTION__,__LINE__), Perimeter::MemCntr::_assertNonNullPtr( Perimeter::MemCntr::reallocMemory( p, sz ), true ))
#define  perimeter_free( p )           do{ Perimeter::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__); Perimeter::MemCntr::freeMemory( Perimeter::MemCntr::_assertNonNullPtr( p, false )); }while(0)


#else

#define  perimeter_new( t, ... )       ( Perimeter::MemCntr::incrNewCntr(1 ), static_cast< t *>( Perimeter::MemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true ) ))
#define  perimeter_delete( o, t )      do{ Perimeter::MemCntr::decrNewCntr(1); delete( static_cast< t *>( Perimeter::MemCntr::_assertNonNullPtr( o, false ) )); }while(0)
#define  perimeter_incr_new_cntr( t )  Perimeter::MemCntr::incrNewCntr(1)
#define  perimeter_decr_new_cntr( t )  Perimeter::MemCntr::decrNewCntr(1)

#define  perimeter_alloc( sz )         ( Perimeter::MemCntr::incrNewCntr(1), Perimeter::MemCntr::_assertNonNullPtr( Perimeter::MemCntr::allocMemory( sz ), true ))
#define  perimeter_realloc( p, sz )    ( Perimeter::MemCntr::incrNewCntr(( p != nullptr ? 0 : 1 )), Perimeter::MemCntr::_assertNonNullPtr( Perimeter::MemCntr::reallocMemory( p, sz ), true ))
#define  perimeter_free( p )           do{ Perimeter::MemCntr::decrNewCntr(1); Perimeter::MemCntr::freeMemory( Perimeter::MemCntr::_assertNonNullPtr( p, false )); }while(0)

#endif

#define  perimeter_curr_new_cntr           ( Perimeter::MemCntr::currNewCntr())
#define  perimeter_enable_mem_trace( sw )  Perimeter::MemCntr::enableMemTrace( sw )
#define  perimeter_enable_mem_cntr( sw )   Perimeter::MemCntr::enableMemCntr( sw )



#ifdef PERIMETER_QT5_ENABLED
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
 *     manual free it use perimeter_delete_qobj( o )
 */
// ============================================================================
#define perimeter_new_qobj( t, ... ) \
    ( \
     [=]( t* _t, const char *fn, const char *func, int ln, const char *type )-> t * { \
         return qobject_cast< t *>( Perimeter::MemCntr::memCntOnQObj( _t, fn, func, ln, type )); \
     } \
    )( new t ( __VA_ARGS__ ), __FILE__, __FUNCTION__, __LINE__, #t )



#define perimeter_delete_qobj( o ) \
    do{ Perimeter::MemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), false ); } while(0)

#define perimeter_delete_qobj_later( o ) \
    do{ Perimeter::MemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), true ); } while(0)

#endif




#endif
