#ifndef QXPACK_IC_AGC_DEF_H
#define QXPACK_IC_AGC_DEF_H

#include <stdint.h>
#include <float.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ////////////////////////////////////////////////////////////////////////////////
                                Export or Import
   ////////////////////////////////////////////////////////////////////////////  */
/* this helper definitions for shared libary support */
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


#ifndef __cplusplus
#define bool  _Bool
#define true  1
#define false 0
#endif



#ifdef __cplusplus
}
#endif
#endif 
