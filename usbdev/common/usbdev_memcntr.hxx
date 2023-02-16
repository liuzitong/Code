#ifndef USBDEV_MEMCNTR_HXX
#define USBDEV_MEMCNTR_HXX
 
#include "usbdev_def.h"

#ifdef USBDEV_QT5_ENABLED
#include <QObject>
#endif


namespace UsbDev {

// ////////////////////////////////////////////////////////////////////////////
//
// memory counter
//
// Tips:
// 1) enable memory trace.
//    define the USBDEV_CFG_MEM_TRACE
//    call  MemCntr::enableMemTrace( ) to enable or disable memory trace
//
// 2) use usbdev_new() / usbdev_delete() to manage the new / delete normal object
//
// 3) use usbdev_new_qobj() / usbdev_delete_qobj() to manage qobject
//
// ////////////////////////////////////////////////////////////////////////////
class USBDEV_API MemCntr {
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
#if defined( USBDEV_QT5_ENABLED )
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

#if defined( USBDEV_CFG_MEM_TRACE )

#define  usbdev_new( t, ... )       ( UsbDev::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ),  static_cast< t *>( UsbDev::MemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true )) )
#define  usbdev_delete( o, t )      do{ UsbDev::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t ); delete( static_cast< t *>( UsbDev::MemCntr::_assertNonNullPtr( o, false )) ); }while(0)
#define  usbdev_incr_new_cntr( t )  UsbDev::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )
#define  usbdev_decr_new_cntr( t )  UsbDev::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__, #t )

#define  usbdev_alloc( sz )         ( UsbDev::MemCntr::incrNewCntr(1,__FILE__,__FUNCTION__,__LINE__), UsbDev::MemCntr::_assertNonNullPtr( UsbDev::MemCntr::allocMemory( sz ), true ))
#define  usbdev_realloc( p, sz )    ( UsbDev::MemCntr::incrNewCntr(( p != nullptr ? 0 : 1 ),__FILE__,__FUNCTION__,__LINE__), UsbDev::MemCntr::_assertNonNullPtr( UsbDev::MemCntr::reallocMemory( p, sz ), true ))
#define  usbdev_free( p )           do{ UsbDev::MemCntr::decrNewCntr(1,__FILE__,__FUNCTION__,__LINE__); UsbDev::MemCntr::freeMemory( UsbDev::MemCntr::_assertNonNullPtr( p, false )); }while(0)


#else

#define  usbdev_new( t, ... )       ( UsbDev::MemCntr::incrNewCntr(1 ), static_cast< t *>( UsbDev::MemCntr::_assertNonNullPtr( new t ( __VA_ARGS__ ), true ) ))
#define  usbdev_delete( o, t )      do{ UsbDev::MemCntr::decrNewCntr(1); delete( static_cast< t *>( UsbDev::MemCntr::_assertNonNullPtr( o, false ) )); }while(0)
#define  usbdev_incr_new_cntr( t )  UsbDev::MemCntr::incrNewCntr(1)
#define  usbdev_decr_new_cntr( t )  UsbDev::MemCntr::decrNewCntr(1)

#define  usbdev_alloc( sz )         ( UsbDev::MemCntr::incrNewCntr(1), UsbDev::MemCntr::_assertNonNullPtr( UsbDev::MemCntr::allocMemory( sz ), true ))
#define  usbdev_realloc( p, sz )    ( UsbDev::MemCntr::incrNewCntr(( p != nullptr ? 0 : 1 )), UsbDev::MemCntr::_assertNonNullPtr( UsbDev::MemCntr::reallocMemory( p, sz ), true ))
#define  usbdev_free( p )           do{ UsbDev::MemCntr::decrNewCntr(1); UsbDev::MemCntr::freeMemory( UsbDev::MemCntr::_assertNonNullPtr( p, false )); }while(0)

#endif

#define  usbdev_curr_new_cntr           ( UsbDev::MemCntr::currNewCntr())
#define  usbdev_enable_mem_trace( sw )  UsbDev::MemCntr::enableMemTrace( sw )
#define  usbdev_enable_mem_cntr( sw )   UsbDev::MemCntr::enableMemCntr( sw )



#ifdef USBDEV_QT5_ENABLED
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
 *     manual free it use usbdev_delete_qobj( o )
 */
// ============================================================================
#define usbdev_new_qobj( t, ... ) \
    ( \
     [=]( t* _t, const char *fn, const char *func, int ln, const char *type )-> t * { \
         return qobject_cast< t *>( UsbDev::MemCntr::memCntOnQObj( _t, fn, func, ln, type )); \
     } \
    )( new t ( __VA_ARGS__ ), __FILE__, __FUNCTION__, __LINE__, #t )



#define usbdev_delete_qobj( o ) \
    do{ UsbDev::MemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), false ); } while(0)

#define usbdev_delete_qobj_later( o ) \
    do{ UsbDev::MemCntr::memCntOnDelObj( qobject_cast<QObject*>(o), true ); } while(0)

#endif




#endif
