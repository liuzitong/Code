#ifndef PERIMETER_DEF_H
#define PERIMETER_DEF_H

#include <stdint.h>
#include <float.h>

#ifdef __cplusplus
	extern "C" {
#endif


/* ////////////////////////////////////////////////////////////////////////////////
                                Export or Import
   ////////////////////////////////////////////////////////////////////////////  */
/* this helper definitions for shared libary support */
#if !defined( PERIMETER_SYM_EXPORT_DEF )
#define PERIMETER_SYM_EXPORT_DEF

#if defined( _WIN32 ) || defined( __CYGWIN__ )
  #define PERIMETER_DLL_IMPORT  __declspec(dllimport)
  #define PERIMETER_DLL_EXPORT  __declspec(dllexport)
  #define PERIMETER_DLL_HIDDEN
#else
  #if __GNUC__ >= 4
     #define PERIMETER_DLL_IMPORT __attribute__ ((visibility ("default")))
     #define PERIMETER_DLL_EXPORT __attribute__ ((visibility ("default")))
     #define PERIMETER_DLL_HIDDEN __attribute__ ((visibility ("hidden")))
  #else
     #define PERIMETER_DLL_IMPORT
     #define PERIMETER_DLL_EXPORT
     #define PERIMETER_DLL_HIDDEN
  #endif
#endif

/* check if defined PERIMETER_DLL */
#if defined( PERIMETER_CFG_STATIC )  && !defined( PERIMETER_CFG_STATIC )  && !defined( PERIMETER_CFG_DLL )
  #define PERIMETER_CFG_STATIC
#endif 
#if defined( PERIMETER_CFG_DLL ) && !defined( PERIMETER_CFG_STATIC )  &&  !defined( PERIMETER_CFG_DLL )
  #define PERIMETER_CFG_DLL
#endif

#undef PERIMETER_API
#undef PERIMETER_HIDDEN
#if defined( PERIMETER_CFG_STATIC )  /* normally build as static library */
  #define PERIMETER_API
  #define PERIMETER_HIDDEN
#else
  #if defined( PERIMETER_CFG_DLL )  /* we are building dll */
    #define PERIMETER_API      PERIMETER_DLL_EXPORT
    #define PERIMETER_HIDDEN   PERIMETER_DLL_HIDDEN
  #else                       /* call() and jump() when load as dll */
    #define PERIMETER_API      PERIMETER_DLL_IMPORT
    #define PERIMETER_HIDDEN
  #endif
#endif

#endif   

#ifdef __cplusplus
	}
#endif


/* output the information */
#if defined( PERIMETER_USE_QT5_INFO )
#  include <QDebug>
#  define PERIMETER_DEBUG  QDebug
#  define PERIMETER_INFO   qInfo
#  define PERIMETER_FATAL  qFatal
#else
#  include <iostream>
#  include <exception>
#  include "perimeter_logging.hxx"
#  define PERIMETER_DEBUG  perimeter_debug
#  define PERIMETER_INFO   perimeter_info
#  define PERIMETER_FATAL  perimeter_fatal
#endif

/* disable the copy in class */
#define PERIMETER_DISABLE_COPY( t ) private : t ( const t & ); t & operator = ( const t & );

#if defined( perimeter_ptrcast )
#  error "perimeter_ptrcast already defined, conflicit ! Abort!"
#else
#  define perimeter_ptrcast( t, o )  ( reinterpret_cast< t >( reinterpret_cast< intptr_t >( o ))  )
#endif

#if defined( perimeter_intptrcast )
#  error "perimeter_intptrcast already defined, conflicit! Abort!"
#else
#  define perimeter_intptrcast( o )  ( reinterpret_cast< intptr_t >( o ))
#endif

#if defined( perimeter_objcast )
#  error "perimeter_objcast already defined, conflict! Abort!"
#else
#  define perimeter_objcast( t, o )  ( static_cast< t >( o ) )
#endif

/* define the unsued macro */
#if !defined( PERIMETER_UNUSED )
#define PERIMETER_UNUSED( x ) ((void)x)
#endif

#endif
