// ////////////////////////////////////////////////////////////////////////////
//! @file     qxpack_sc_memoryblock.cxx
//! @brief    this is a portable memory block manager
//! @author   night wing
//! @date     2020/08
//! @version  0.3.0 ( modified for indcom )
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_SC_MEMBLOCK_HXX
#define QXPACK_SC_MEMBLOCK_HXX

#include "qxpack_ic_def.h"
#include <cstdlib>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <cstring>


// ////////////////////////////////////////////////////////////////////////////
// config. section,
// ////////////////////////////////////////////////////////////////////////////
//#define  QXPACK_SC_CFG_STATIC   // using source with project
//#defnie  QXPACK_SC_CFG_DLL    // build as DLL and export it with DLL

#if    defined( QXPACK_IC_CFG_STATIC ) || defined( QXPACK_IC_ALGOR_CFG_STATIC )
#  define QXPACK_SC_CFG_STATIC
#elif  defined( QXPACK_IC_CFG_DLL ) || defined( QXPACK_IC_ALGOR_CFG_DLL )
#  define QXPACK_SC_CFG_DLL
#else
#  error "please ensure CFG_STATIC or CFG_DLL!"
#endif

// ////////////////////////////////////////////////////////////////////////////
//                    pre-check marcos
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {

#if !defined( QXPACK_SC_SYM_EXPORT_DEF )  // start of QXPACK_SC_SYM_EXPORT_DEF
#define QXPACK_SC_SYM_EXPORT_DEF

// check if this compiler system support c++11
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1900 )
#  define QXPACK_SC_CXX11
#else
#  error "this compiler system is not support c++11!"
#endif

#ifdef QXPACK_SC_CXX11
#  define QXPACK_SC_STATIC_ASSERT  static_assert
#  define QXPACK_SC_FINAL          final
#else
#  define QXPACK_SC_STATIC_ASSERT  assert
#  define QXPACK_SC_FINAL
#endif

// since 0.2.0
#define QXPACK_SC_FATAL( c, ... ) \
   do{ if( !(c) ) { std::fprintf( stderr, "%s::%s() ",__FILE__, __FUNCTION__ ); std::fprintf( stderr, __VA_ARGS__ ); std::abort(); }}while(0)

#if defined(_NDEBUG) || defined( NDEBUG )
#define QXPACK_SC_ASSERT( c, ... )
#else
#define QXPACK_SC_ASSERT( c, ... ) \
   do{ if( !(c) ) { std::fprintf( stderr, "%s::%s() ",__FILE__, __FUNCTION__ ); std::fprintf( stderr, __VA_ARGS__ ); std::abort(); }}while(0)
#endif

#define QXPACK_SC_UNUSED( o ) ((void)(o))

// the export and import macro
#if defined( _WIN32 ) || defined( __CYGWIN__ )
  #define QXPACK_SC_DLL_IMPORT  __declspec(dllimport)
  #define QXPACK_SC_DLL_EXPORT  __declspec(dllexport)
  #define QXPACK_SC_DLL_HIDDEN
#else
  #if __GNUC__ >= 4
     #define QXPACK_SC_DLL_IMPORT __attribute__ ((visibility ("default")))
     #define QXPACK_SC_DLL_EXPORT __attribute__ ((visibility ("default")))
     #define QXPACK_SC_DLL_HIDDEN __attribute__ ((visibility ("hidden")))
  #else
     #define QXPACK_SC_DLL_IMPORT
     #define QXPACK_SC_DLL_EXPORT
     #define QXPACK_SC_DLL_HIDDEN
  #endif
#endif

#undef QXPACK_SC_API
#undef QXPACK_SC_HIDDEN
#if defined( QXPACK_SC_CFG_STATIC )  /* normally build as static library, if we build  */
#  define QXPACK_SC_API
#  define QXPACK_SC_HIDDEN
#else
#  if defined( QXPACK_SC_CFG_DLL )  /* we are building dll */
#      define QXPACK_SC_API      QXPACK_SC_DLL_EXPORT
#      define QXPACK_SC_HIDDEN   QXPACK_SC_DLL_HIDDEN
#  else                           /* call() and jump() when load as dll */
#      define QXPACK_SC_API      QXPACK_SC_DLL_IMPORT
#      define QXPACK_SC_HIDDEN
#  endif
#endif


#endif // end of QXPACK_SC_SYM_EXPORT_DEF
}


// ////////////////////////////////////////////////////////////////////////////
//
// the memory block class, used to provide memory manage ( final version )
//
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {

class  QXPACK_SC_API  ScMemBlock  QXPACK_SC_FINAL {

private: 
    void *m_obj;  // DO NOT add member in future
public :   
    explicit ScMemBlock();
    ScMemBlock( size_t sz );
    ~ScMemBlock();
    ScMemBlock( const ScMemBlock & );
    ScMemBlock & operator = ( const ScMemBlock & );

#ifdef QXPACK_SC_CXX11
    ScMemBlock ( ScMemBlock &&ot ) { m_obj = ot.m_obj; ot.m_obj = nullptr; }
    ScMemBlock & operator = ( ScMemBlock &&ot ) { if ( this != &ot ) { makeNull(); m_obj = ot.m_obj; ot.m_obj = nullptr; } return *this; }
#endif

    // since 0.2.0
    enum  ActID { ActID_Init = 0x0, ActID_Destroy, ActID_InitByCopy };
    struct QXPACK_SC_HIDDEN  ActInitParam        QXPACK_SC_FINAL { void *data; size_t data_sz; };
    struct QXPACK_SC_HIDDEN  ActDestroyParam     QXPACK_SC_FINAL { void *data; size_t data_sz; };
    struct QXPACK_SC_HIDDEN  ActInitByCopyParam  QXPACK_SC_FINAL { void *tgt_data; size_t tgt_sz; void *src_data; size_t src_sz; };
    typedef bool (*UserProc)( ActID act_id, void *act_param, void*);
    ScMemBlock( size_t sz, UserProc, void* = nullptr );

    //! @brief return current version of this ScMemBlock
    size_t        version() const;

    //! @brief make this object null
    //! @since 0.1.0
    void          makeNull();

    //! @brief check if this object is null or not.
    //! @return true means this object is null, no memory associated.
    inline bool   isNull()  const { return m_obj == nullptr; }

    //! @since 0.2.0
    inline bool   isValid() const { return m_obj != nullptr; }

    //! @return the memory size of this block hold
    size_t        size() const;

    //! @brief resize the memory size
    //! @since 0.2.0
    //! @note if sz is zero, this memory will be make null; \n
    //!       if 'sz' less than orginal size, the content will be tripped; \n
    //!       if current object is shared with other, it will be do a copy and resize; \n
    //!       if the object is null, resize will allocate spec. size
    void          resize( size_t sz );

    //! @return the memory pointer
    const void*   constData() const;    // directly visit the memory data pointer

    //! @return a duplicate object of this block
    ScMemBlock    copy() const;         // make a copy of this block

    //! @brief  duplicate if shared with other
    //! @return true means this object is duplicated.
    bool          dupIfShared();

    //! @brief  return writable data pointer and make a copy if shared
    //! @note   this function will do a deep copy if this object shared with other
    //! @since  0.1.0
    //! @warning this function DO A DEEP COPY if shared!
    void*         dataCow();

    //! @brief library inner used. checking the current memblock counter.
    static intptr_t  currMemCntr();     // NOTE: library inner used only.

    //! @brief move target object pointer to self. self release the refer.
    //! @since 0.1.0
    inline void   movePtrFrom( ScMemBlock &ot )
    { if ( m_obj != nullptr ){ makeNull(); } m_obj = ot.m_obj; ot.m_obj = nullptr; }

    //! @brief check if this block tail is broken
    //! @since 0.3.0
    bool    isTailBroken() const;

    //! @brief check if current is shared with other
    //! @since 0.3.0
    bool    isSharedWithOther() const;

    //! @brief swap memory
    //! @since 0.3.0
    inline void  swap( ScMemBlock &ot )
    { void *tmp = m_obj; m_obj = ot.m_obj; ot.m_obj = tmp;  }
};
}



// ////////////////////////////////////////////////////////////////////////////
//
//                      the template helper functions ( since 0.2.0 )
//
// ////////////////////////////////////////////////////////////////////////////
#include <new>
#include <type_traits>
#include <utility>

namespace QxPack {
namespace ScMemBlockHelper {

// ============================================================================
// deInitT()/deInitOneT(),  used to call class's dtor
// 1) has virtual dtor
// 2) has default dtor
// ============================================================================
template <typename T>
inline typename  std::enable_if<std::is_destructible<T>::value>::type
__deInitOneT2( T &dp ) { dp.~T(); }

template <typename T>
inline typename  std::enable_if<!std::is_destructible<T>::value>::type
__deInitOneT2( T &dp ) { QXPACK_SC_UNUSED(dp); } // DO NOTHING

template <typename T>
inline typename  std::enable_if<std::has_virtual_destructor<T>::value>::type
deInitOneT( T &dp ) { dp.~T(); }

template <typename T>
inline typename  std::enable_if<!std::has_virtual_destructor<T>::value>::type
deInitOneT( T &dp ) { __deInitOneT2<T>( dp ); }

template <typename T>
inline typename  std::enable_if<std::has_virtual_destructor<T>::value>::type
deInitT( T *dp, size_t num )
{
    QXPACK_SC_ASSERT( dp != nullptr, "nullptr pointer!");
    for ( size_t i = 0; i < num; i ++ ) { dp[i].~T(); }
}

template <typename T>
inline typename  std::enable_if<!std::has_virtual_destructor<T>::value>::type
deInitT( T *dp, size_t num )
{
    QXPACK_SC_ASSERT( dp != nullptr, "nullptr pointer!");
    for ( size_t i = 0; i < num; i ++ ) { __deInitOneT2<T>( dp[i] ); }
}


// ===========================================================================
// initT()/initOne(), used to call class' default ctor
// ===========================================================================
template <typename T>
inline typename  std::enable_if<std::is_default_constructible<T>::value>::type
initOneT( T &dp ) { new( &dp ) T; }

template <typename T>
inline typename  std::enable_if<!std::is_default_constructible<T>::value>::type
initOneT( T &dp )
{
    QXPACK_SC_UNUSED(dp);
    QXPACK_SC_STATIC_ASSERT( false, "no default constructor!" );
}

template <typename T>
inline typename  std::enable_if<std::is_default_constructible<T>::value>::type
initT( T *dp, size_t num )
{
    QXPACK_SC_ASSERT( dp != nullptr, "null pointer!");
    for ( size_t i = 0; i < num; i ++ ) { new( dp + i ) T; }
}

template <typename T>
inline typename  std::enable_if<!std::is_default_constructible<T>::value>::type
initT( T *dp, size_t num )
{
   QXPACK_SC_UNUSED(dp); QXPACK_SC_UNUSED(num);
   QXPACK_SC_STATIC_ASSERT( false, "no default constructor!" );
}


// ============================================================================
// initByCopyT(), used to do class copy
// The call priorities are as follows:
//  1) has copy constructor
//  2) has copy assign
// ============================================================================
template <typename T>
inline typename  std::enable_if<std::is_copy_assignable<T>::value>::type
__initOneByAssignCopyT( T &dst, const T &src ) { new( &dst ) T; dst = src; }

template <typename T>
inline typename  std::enable_if<!std::is_copy_assignable<T>::value>::type
__initOneByAssignCopyT( T &dst, const T &src )
{
    QXPACK_SC_UNUSED(dst); QXPACK_SC_UNUSED(src);
    QXPACK_SC_STATIC_ASSERT( false, "no 'operator =' found!");
}

template <typename T>
inline typename std::enable_if<std::is_copy_constructible<T>::value>::type
__initOneByCtorCopyT( T &dst, const T &src ) { new( &dst ) T( src ); }

template <typename T>
inline typename std::enable_if<!std::is_copy_constructible<T>::value>::type
__initOneByCtorCopyT( T &dst, const T &src )
{
    QXPACK_SC_UNUSED(dst); QXPACK_SC_UNUSED(src);
    QXPACK_SC_STATIC_ASSERT( false, "no 'copy constructor' found!");
}

template <typename T>
inline typename std::enable_if<std::is_copy_constructible<T>::value>::type
initByCopyT( T *dst, size_t dst_num, const T *src, size_t src_num  )
{
    QXPACK_SC_ASSERT( dst != nullptr && src != nullptr, "null pointer!");
    size_t min_num = ( dst_num <= src_num ? dst_num : src_num );
    for ( size_t i = 0; i < min_num; i ++ ) { __initOneByCtorCopyT<T>( dst[i], src[i] ); }
    initT<T>( dst + min_num, dst_num - min_num );
}

template <typename T>
inline typename std::enable_if<!std::is_copy_constructible<T>::value>::type
initByCopyT( T *dst, size_t dst_num, const T *src, size_t src_num )
{
    QXPACK_SC_ASSERT( dst != nullptr && src != nullptr, "null pointer!");
    size_t min_num = ( dst_num <= src_num ? dst_num : src_num );
    for ( size_t i = 0; i < min_num; i ++ ) { __initOneByAssignCopyT<T>( dst[i], src[i] ); }
    initT<T>( dst + min_num, dst_num - min_num );
}

template <typename T>
inline typename std::enable_if<std::is_copy_constructible<T>::value>::type
initOneByCopyT( T &dst, const T &src ) { __initOneByCtorCopyT<T>( dst, src ); }

template <typename T>
inline typename std::enable_if<!std::is_copy_constructible<T>::value>::type
initOneByCopyT( T &dst, const T &src ) { __initOneByAssignCopyT<T>( dst, src ); }



// ============================================================================
// initByMoveT()
// 1) has move ctor
// 2) has move assign
// ============================================================================
template <typename T> // the dst is not init.
inline typename std::enable_if<std::is_move_constructible<T>::value>::type
__initOneByMoveCtorT( T &dst,  T &src ) { new( &dst ) T( std::move( src )); }

template <typename T> // the dst is not init.
inline typename std::enable_if<!std::is_move_constructible<T>::value>::type
__initOneByMoveCtorT( T &dst,  T &src )
{
    // fallback operation
    ScMemBlockHelper::initOneByCopyT<T>( dst, src );
    ScMemBlockHelper::deInitOneT<T>( src );
    ScMemBlockHelper::initOneT<T>( src );
}

template <typename T> // the dst it not init
inline typename std::enable_if<std::is_move_assignable<T>::value>::type
__initOneByMoveAssignT( T &dst, T &src ) { new( &dst ) T; dst = std::move( src ); }

template <typename T> // the dst it not init
inline typename std::enable_if<!std::is_move_assignable<T>::value>::type
__initOneByMoveAssignT( T &dst, T &src )
{
    // fallback operation
    ScMemBlockHelper::initOneByCopyT<T>( dst, src );
    ScMemBlockHelper::deInitOneT<T>( src );
    ScMemBlockHelper::initOneT<T>( src );
}

template <typename T>
inline typename std::enable_if<std::is_move_constructible<T>::value>::type
initByMoveT( T *dst, T *src, size_t num )
{
    QXPACK_SC_ASSERT( dst != nullptr && src != nullptr, "null pointer!" );
    for ( size_t i = 0; i < num; i ++ ) { __initOneByMoveCtorT<T>( dst[i], src[i] ); }
}

template <typename T>
inline typename std::enable_if<!std::is_move_constructible<T>::value>::type
initByMoveT( T *dst, T *src, size_t num )
{
    QXPACK_SC_ASSERT( dst != nullptr && src != nullptr, "null pointer!");
    for ( size_t i = 0; i < num; i ++ ) { __initOneByMoveAssignT<T>( dst[i], src[i] ); }
}

template <typename T>
inline typename std::enable_if<std::is_move_constructible<T>::value>::type
initOneByMoveT( T &dst, T &src ) { __initOneByMoveCtorT<T>( dst, src ); }

template <typename T>
inline typename std::enable_if<!std::is_move_constructible<T>::value>::type
initOneByMoveT( T &dst, T &src ) { __initOneByMoveAssignT<T>( dst, src ); }


// ============================================================================
// takeByMoveT()
// 1) has move ctor
// 2) has move assign
// ============================================================================
template <typename T> // the dst has been init.
inline typename std::enable_if<std::is_copy_assignable<T>::value>::type
__takeOneByCopyAssignT( T &dst, T &src ) { dst = src; src = T; }

template <typename T> // the dst has been init.
inline typename std::enable_if<!std::is_copy_assignable<T>::value>::type
__takeOneByCopyAssignT( T &dst, T &src )
{
    // fallback operation
    deInitOneT<T>( dst );
    initOneByMoveT<T>( dst, src );
}

template <typename T> // dst has been init.
inline typename std::enable_if<std::is_move_constructible<T>::value>::type
__takeOneByMoveCtorT( T &dst, T &src )
{
    deInitOneT<T>( dst );
    initOneByMoveT<T>( dst, src );
}

template <typename T> // dst has been init.
inline typename std::enable_if<!std::is_move_constructible<T>::value>::type
__takeOneByMoveCtorT( T &dst, T &src )
{ __takeOneByCopyAssignT<T>( dst, src ); }

template <typename T> // the dst has been init.
inline typename std::enable_if<std::is_move_assignable<T>::value>::type
takeOneByMoveT( T &dst,  T &src ) { dst = std::move( src ); }

template <typename T> // the dst has been init.
inline typename std::enable_if<!std::is_move_assignable<T>::value>::type
takeOneByMoveT( T &dst,  T &src ) { __takeOneByMoveCtorT<T>( dst, src ); }



// ============================================================================
// others
// ============================================================================
inline void*  ptrAddHdr( void *p, size_t sz ) { return reinterpret_cast<uint8_t*>( p ) + sz; }

template <typename T>
inline T      clamp( const T&v, const T&min, const T&max )
{ return ( v > min ? ( v < max ? v : max ) : min ) ; }

template <typename T>
void  verifyClass( )
{
    QXPACK_SC_FATAL( std::is_default_constructible<T>::value, "the class type require default constructor!" );
    QXPACK_SC_FATAL(
        std::has_virtual_destructor<T>::value || std::is_destructible<T>::value,
        "the class need destructor!"
    );
    QXPACK_SC_FATAL(
        std::is_copy_assignable<T>::value || std::is_copy_constructible<T>::value,
        "the class type require copy_assignable or copy_constructible."
    );
}

}
}


#endif 
