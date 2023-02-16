#ifndef USBDEV_DEF_H
#define USBDEV_DEF_H

#include <stdint.h>
#include <float.h>

#ifdef __cplusplus
	extern "C" {
#endif


/* ////////////////////////////////////////////////////////////////////////////////
                                Export or Import
   ////////////////////////////////////////////////////////////////////////////  */
/* this helper definitions for shared libary support */
#if !defined( USBDEV_SYM_EXPORT_DEF )
#define USBDEV_SYM_EXPORT_DEF

#if defined( _WIN32 ) || defined( __CYGWIN__ )
  #define USBDEV_DLL_IMPORT  __declspec(dllimport)
  #define USBDEV_DLL_EXPORT  __declspec(dllexport)
  #define USBDEV_DLL_HIDDEN
#else
  #if __GNUC__ >= 4
     #define USBDEV_DLL_IMPORT __attribute__ ((visibility ("default")))
     #define USBDEV_DLL_EXPORT __attribute__ ((visibility ("default")))
     #define USBDEV_DLL_HIDDEN __attribute__ ((visibility ("hidden")))
  #else
     #define USBDEV_DLL_IMPORT
     #define USBDEV_DLL_EXPORT
     #define USBDEV_DLL_HIDDEN
  #endif
#endif

/* check if defined USBDEV_DLL */
#if defined( USBDEV_CFG_STATIC )  && !defined( USBDEV_CFG_STATIC )  && !defined( USBDEV_CFG_DLL )
  #define USBDEV_CFG_STATIC
#endif 
#if defined( USBDEV_CFG_DLL ) && !defined( USBDEV_CFG_STATIC )  &&  !defined( USBDEV_CFG_DLL )
  #define USBDEV_CFG_DLL
#endif

#undef USBDEV_API
#undef USBDEV_HIDDEN
#if defined( USBDEV_CFG_STATIC )  /* normally build as static library */
  #define USBDEV_API
  #define USBDEV_HIDDEN
#else
  #if defined( USBDEV_CFG_DLL )  /* we are building dll */
    #define USBDEV_API      USBDEV_DLL_EXPORT
    #define USBDEV_HIDDEN   USBDEV_DLL_HIDDEN
  #else                       /* call() and jump() when load as dll */
    #define USBDEV_API      USBDEV_DLL_IMPORT
    #define USBDEV_HIDDEN
  #endif
#endif

#endif   

#ifdef __cplusplus
	}
#endif


/* output the information */
#if defined( USBDEV_USE_QT5_INFO )
#  include <QDebug>
#  define USBDEV_DEBUG  QDebug
#  define USBDEV_INFO   qInfo
#  define USBDEV_FATAL  qFatal
#else
#  include <iostream>
#  include <exception>
#  include "usbdev_logging.hxx"
#  define USBDEV_DEBUG  usbdev_debug
#  define USBDEV_INFO   usbdev_info
#  define USBDEV_FATAL  usbdev_fatal
#endif

/* disable the copy in class */
#define USBDEV_DISABLE_COPY( t ) private : t ( const t & ); t & operator = ( const t & );

#if defined( usbdev_ptrcast )
#  error "usbdev_ptrcast already defined, conflicit ! Abort!"
#else
#  define usbdev_ptrcast( t, o )  ( reinterpret_cast< t >( reinterpret_cast< intptr_t >( o ))  )
#endif

#if defined( usbdev_intptrcast )
#  error "usbdev_intptrcast already defined, conflicit! Abort!"
#else
#  define usbdev_intptrcast( o )  ( reinterpret_cast< intptr_t >( o ))
#endif

#if defined( usbdev_objcast )
#  error "usbdev_objcast already defined, conflict! Abort!"
#else
#  define usbdev_objcast( t, o )  ( static_cast< t >( o ) )
#endif

/* define the unsued macro */
#if !defined( USBDEV_UNUSED )
#define USBDEV_UNUSED( x ) ((void)x)
#endif

#endif
