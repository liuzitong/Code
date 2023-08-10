#ifndef QXPACK_IC_DEF_H
#define QXPACK_IC_DEF_H

#include <cstdint>
#include <cfloat>
#include <cstdio>

/* ////////////////////////////////////////////////////////////////////////////////

                        Export or Import

   ////////////////////////////////////////////////////////////////////////////// */
#if !defined( QXPACK_IC_SYM_EXPORT_DEF )
#define QXPACK_IC_SYM_EXPORT_DEF

#if defined( _WIN32 ) || defined( __CYGWIN__ )
  #define QXPACK_IC_DLL_IMPORT  __declspec(dllimport)
  #define QXPACK_IC_DLL_EXPORT  __declspec(dllexport)
  #define QXPACK_IC_DLL_HIDDEN
#else
  #if __GNUC__ >= 4
     #define QXPACK_IC_DLL_IMPORT __attribute__ ((visibility ("default")))
     #define QXPACK_IC_DLL_EXPORT __attribute__ ((visibility ("default")))
     #define QXPACK_IC_DLL_HIDDEN __attribute__ ((visibility ("hidden")))
  #else
     #define QXPACK_IC_DLL_IMPORT
     #define QXPACK_IC_DLL_EXPORT
     #define QXPACK_IC_DLL_HIDDEN
  #endif
#endif

/* check if defined QXPACK_IC_DLL */
#if defined( QXPACK_CFG_STATIC )  && !defined( QXPACK_IC_CFG_STATIC )  && !defined( QXPACK_IC_CFG_DLL )
  #define QXPACK_IC_CFG_STATIC
#endif 
#if defined( QXPACK_CFG_DLL ) && !defined( QXPACK_IC_CFG_STATIC )  &&  !defined( QXPACK_IC_CFG_DLL )
  #define QXPACK_IC_CFG_DLL
#endif

#undef QXPACK_IC_API
#undef QXPACK_IC_HIDDEN
#if defined( QXPACK_IC_CFG_STATIC )  /* normally build as static library */ 
  #define QXPACK_IC_API
  #define QXPACK_IC_HIDDEN
#else
  #if defined( QXPACK_IC_CFG_DLL )  /* we are building dll */
    #define QXPACK_IC_API      QXPACK_IC_DLL_EXPORT
    #define QXPACK_IC_HIDDEN   QXPACK_IC_DLL_HIDDEN
  #else                       /* call() and jump() when load as dll */
    #define QXPACK_IC_API      QXPACK_IC_DLL_IMPORT  
    #define QXPACK_IC_HIDDEN  
  #endif
#endif

#endif   

/* ////////////////////////////////////////////////////////////////////////////

                        Env.

   ///////////////////////////////////////////////////////////////////////// */
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#  define QXPACK_IC_CXX11
#else
#  undef  QXPACK_IC_CXX11
#endif

#if !defined( QXPACK_IC_UNUSED )
#  define QXPACK_IC_UNUSED( x ) ((void)x)
#endif


/* ////////////////////////////////////////////////////////////////////////////

                     debug information

   ///////////////////////////////////////////////////////////////////////// */
#ifdef __cplusplus

#if defined( QXPACK_IC_USE_QT5_INFO )
#  include <QDebug>
#  define QXPACK_IC_DEBUG  QDebug
#  define QXPACK_IC_INFO   qInfo
#  define QXPACK_IC_FATAL  qFatal
#else
#  include <iostream>
#  include <exception>
#  include "qxpack_ic_logging.hxx"
#  define QXPACK_IC_DEBUG  qxpack_ic_debug
#  define QXPACK_IC_INFO   qxpack_ic_info
#  define QXPACK_IC_FATAL  qxpack_ic_fatal
#endif

#define QXPACK_IC_DISABLE_COPY( t ) private : t ( const t & ); t & operator = ( const t & );

#if defined( T_PtrCast )
#  error "T_PtrCast already defined, conflicit ! Abort!"
#else
#  define T_PtrCast( t, o )  ( reinterpret_cast< t >( reinterpret_cast< intptr_t >( o ))  )
#endif
#if defined( T_IntPtrCast )
#  error "T_IntPtrCast already defined, conflicit! Abort!"
#else
#  define T_IntPtrCast( o )  ( reinterpret_cast< intptr_t >( o ))
#endif
#if defined( T_ObjCast )
#  error "T_ObjCast already defined, conflict! Abort!"
#else
#  define T_ObjCast( t, o )  ( static_cast< t >( o ) )
#endif

#ifdef QXPACK_IC_CXX11
#  define QXPACK_IC_STATIC_ASSERT  static_assert
#  define QXPACK_IC_FINAL          final
#else
#  define QXPACK_IC_STATIC_ASSERT  assert
#  define QXPACK_IC_FINAL
#endif

#if defined(_NDEBUG) || defined( NDEBUG )
#  define QXPACK_IC_ASSERT( c, msg, ... )
#else
#  define QXPACK_IC_ASSERT( c, msg, ... ) \
   do{ if( !(c) ) { qxpack_ic_fatal( #c " " msg, ## __VA_ARGS__ ); }}while(0)

#endif


#else   /* if include by pure C */
# error "current doest not support pure C!"

#endif





/* ////////////////////////////////////////////////////////////////////////////

                        the predefined type structure

   ///////////////////////////////////////////////////////////////////////// */
#ifndef QXPACK_PARAM_HINT
#define QXPACK_PARAM_HINT

#  define qx_in
#  define qx_in_opt
#  define qx_out
#  define qx_out_opt
#  define qx_in_out
#  define qx_in_out_opt
#  define qx_opt_in
#  define qx_opt_out
#  define qx_opt_in_out
#  define qx_in_opt_out

#endif

#ifndef QXPACK_TYPE_QXREAL_T
#  define QXPACK_TYPE_QXREAL_T
   typedef double qxreal_t;
#  define QXPACK_TYPE_QXREAL_MAX       DBL_MAX
#  define QXPACK_TYPE_QXREAL_MIN       DBL_MIN
#endif

// is defined qxvec2d_t ?
#ifndef QXPACK_TYPE_QXVEC2D_T
#  define QXPACK_TYPE_QXVEC2D_T
   typedef qxreal_t qxvec2d_real_t;

#    ifdef __cplusplus
   struct qxvec2d_t { // for C++, can use ctor()
      qxvec2d_real_t x, y;
      qxvec2d_t( qxreal_t _x, qxreal_t _y ) : x(_x), y(_y){}
      qxvec2d_t( ) : x(0), y(0) {}
   };
#    else
   typedef struct {  qxvec2d_real_t x, y; } qxvec2d_t;
#    endif

#  define QXPACK_TYPE_QXVEC2D_REAL_MAX       QXPACK_TYPE_QXREAL_MAX
#  define QXPACK_TYPE_QXVEC2D_REAL_MIN       QXPACK_TYPE_QXREAL_MIN
#endif

// is defined qxpt2d_t ?
#ifndef QXPACK_TYPE_QXPT2D_T
#define QXPACK_TYPE_QXPT2D_T
typedef qxreal_t   qxpt2d_real_t;
typedef qxvec2d_t  qxpt2d_t;
#define QXPACK_TYPE_QXPT2D_REAL_MAX       QXPACK_TYPE_QXREAL_MAX
#define QXPACK_TYPE_QXPT2D_REAL_MIN       QXPACK_TYPE_QXREAL_MIN
#endif





#endif
