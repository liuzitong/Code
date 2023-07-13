#ifndef JRPCPLAT_DEF_H
#define JRPCPLAT_DEF_H

#include <stdint.h>
#include <float.h>

#ifdef __cplusplus
	extern "C" {
#endif


/* ////////////////////////////////////////////////////////////////////////////////
                                Export or Import
   ////////////////////////////////////////////////////////////////////////////  */
/* this helper definitions for shared libary support */
#if !defined( JRPCPLAT_SYM_EXPORT_DEF )
#define JRPCPLAT_SYM_EXPORT_DEF

#if defined( _WIN32 ) || defined( __CYGWIN__ )
  #define JRPCPLAT_DLL_IMPORT  __declspec(dllimport)
  #define JRPCPLAT_DLL_EXPORT  __declspec(dllexport)
  #define JRPCPLAT_DLL_HIDDEN
#else
  #if __GNUC__ >= 4
     #define JRPCPLAT_DLL_IMPORT __attribute__ ((visibility ("default")))
     #define JRPCPLAT_DLL_EXPORT __attribute__ ((visibility ("default")))
     #define JRPCPLAT_DLL_HIDDEN __attribute__ ((visibility ("hidden")))
  #else
     #define JRPCPLAT_DLL_IMPORT
     #define JRPCPLAT_DLL_EXPORT
     #define JRPCPLAT_DLL_HIDDEN
  #endif
#endif

/* check if defined JRPCPLAT_DLL */
#if defined( JRPCPLAT_CFG_STATIC )  && !defined( JRPCPLAT_CFG_STATIC )  && !defined( JRPCPLAT_CFG_DLL )
  #define JRPCPLAT_CFG_STATIC
#endif 
#if defined( JRPCPLAT_CFG_DLL ) && !defined( JRPCPLAT_CFG_STATIC )  &&  !defined( JRPCPLAT_CFG_DLL )
  #define JRPCPLAT_CFG_DLL
#endif

#undef JRPCPLAT_API
#undef JRPCPLAT_HIDDEN
#if defined( JRPCPLAT_CFG_STATIC )  /* normally build as static library */
  #define JRPCPLAT_API
  #define JRPCPLAT_HIDDEN
#else
  #if defined( JRPCPLAT_CFG_DLL )  /* we are building dll */
    #define JRPCPLAT_API      JRPCPLAT_DLL_EXPORT
    #define JRPCPLAT_HIDDEN   JRPCPLAT_DLL_HIDDEN
  #else                       /* call() and jump() when load as dll */
    #define JRPCPLAT_API      JRPCPLAT_DLL_IMPORT
    #define JRPCPLAT_HIDDEN
  #endif
#endif

#endif   

#ifdef __cplusplus
	}
#endif


/* output the information */
#if defined( JRPCPLAT_USE_QT5_INFO )
#  include <QDebug>
#  define JRPCPLAT_DEBUG  QDebug
#  define JRPCPLAT_INFO   qInfo
#  define JRPCPLAT_FATAL  qFatal
#else
#  include <iostream>
#  include <exception>
#  include "jrpcplat_logging.hxx"
#  define JRPCPLAT_DEBUG  jrpcplat_debug
#  define JRPCPLAT_INFO   jrpcplat_info
#  define JRPCPLAT_FATAL  jrpcplat_fatal
#endif

/* disable the copy in class */
#define JRPCPLAT_DISABLE_COPY( t ) private : t ( const t & ); t & operator = ( const t & );

#if defined( jrpcplat_ptrcast )
#  error "jrpcplat_ptrcast already defined, conflicit ! Abort!"
#else
#  define jrpcplat_ptrcast( t, o )  ( reinterpret_cast< t >( reinterpret_cast< intptr_t >( o ))  )
#endif

#if defined( jrpcplat_intptrcast )
#  error "jrpcplat_intptrcast already defined, conflicit! Abort!"
#else
#  define jrpcplat_intptrcast( o )  ( reinterpret_cast< intptr_t >( o ))
#endif

#if defined( jrpcplat_objcast )
#  error "jrpcplat_objcast already defined, conflict! Abort!"
#else
#  define jrpcplat_objcast( t, o )  ( static_cast< t >( o ) )
#endif

/* define the unsued macro */
#if !defined( JRPCPLAT_UNUSED )
#define JRPCPLAT_UNUSED( x ) ((void)x)
#endif

// C++11 macro support
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1900 )
#define JRPCPLAT_CXX11
#else
#endif


#endif
