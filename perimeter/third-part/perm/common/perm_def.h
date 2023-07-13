#ifndef PERM_DEF_H
#define PERM_DEF_H

#include <stdint.h>
#include <float.h>

#ifdef __cplusplus
	extern "C" {
#endif


/* ////////////////////////////////////////////////////////////////////////////////
                                Export or Import
   ////////////////////////////////////////////////////////////////////////////  */
/* this helper definitions for shared libary support */
#if !defined( PERM_SYM_EXPORT_DEF )
#define PERM_SYM_EXPORT_DEF

#if defined( _WIN32 ) || defined( __CYGWIN__ )
  #define PERM_DLL_IMPORT  __declspec(dllimport)
  #define PERM_DLL_EXPORT  __declspec(dllexport)
  #define PERM_DLL_HIDDEN
#else
  #if __GNUC__ >= 4
     #define PERM_DLL_IMPORT __attribute__ ((visibility ("default")))
     #define PERM_DLL_EXPORT __attribute__ ((visibility ("default")))
     #define PERM_DLL_HIDDEN __attribute__ ((visibility ("hidden")))
  #else
     #define PERM_DLL_IMPORT
     #define PERM_DLL_EXPORT
     #define PERM_DLL_HIDDEN
  #endif
#endif

/* check if defined PERM_DLL */
#if defined( PERM_CFG_STATIC )  && !defined( PERM_CFG_STATIC )  && !defined( PERM_CFG_DLL )
  #define PERM_CFG_STATIC
#endif 
#if defined( PERM_CFG_DLL ) && !defined( PERM_CFG_STATIC )  &&  !defined( PERM_CFG_DLL )
  #define PERM_CFG_DLL
#endif

#undef PERM_API
#undef PERM_HIDDEN
#if defined( PERM_CFG_STATIC )  /* normally build as static library */
  #define PERM_API
  #define PERM_HIDDEN
#else
  #if defined( PERM_CFG_DLL )  /* we are building dll */
    #define PERM_API      PERM_DLL_EXPORT
    #define PERM_HIDDEN   PERM_DLL_HIDDEN
  #else                       /* call() and jump() when load as dll */
    #define PERM_API      PERM_DLL_IMPORT
    #define PERM_HIDDEN
  #endif
#endif

#endif   

#ifdef __cplusplus
	}
#endif


/* C++11 macro support */
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1900 )
#define PERM_CXX11
#else
#endif

#ifdef PERM_CXX11
#  define PERM_STATIC_ASSERT  static_assert
#  define PERM_FINAL          final
#else
#  define PERM_STATIC_ASSERT  assert
#  define PERM_FINAL
#endif


/* output the information */
#if defined( PERM_USE_QT5_INFO )
#  include <QDebug>
#  define PERM_DEBUG  QDebug
#  define PERM_INFO   qInfo
#  define PERM_FATAL  qFatal
#else
#  include <iostream>
#  include <exception>
#  include "perm_logging.hxx"
#  define PERM_DEBUG  perm_debug
#  define PERM_INFO   perm_info
#  define PERM_FATAL  perm_fatal
#endif

/* disable the copy in class */
#define PERM_DISABLE_COPY( t ) private : t ( const t & ); t & operator = ( const t & );

#if defined( perm_ptrcast )
#  error "perm_ptrcast already defined, conflicit ! Abort!"
#else
#  define perm_ptrcast( t, o )  ( reinterpret_cast< t >( reinterpret_cast< intptr_t >( o ))  )
#endif

#if defined( perm_intptrcast )
#  error "perm_intptrcast already defined, conflicit! Abort!"
#else
#  define perm_intptrcast( o )  ( reinterpret_cast< intptr_t >( o ))
#endif

#if defined( perm_objcast )
#  error "perm_objcast already defined, conflict! Abort!"
#else
#  define perm_objcast( t, o )  ( static_cast< t >( o ) )
#endif

/* define the unsued macro */
#if !defined( PERM_UNUSED )
#define PERM_UNUSED( x ) ((void)x)
#endif

#endif
