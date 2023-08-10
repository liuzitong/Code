#ifndef QXPACK_IC_ARCH_DEF_H
#define QXPACK_IC_ARCH_DEF_H

// ////////////////////////////////////////////////////////////////////////////////////
//                      architectures
// ////////////////////////////////////////////////////////////////////////////////////
#if !defined( QXPACK_ARCH_DEFINED )
#define QXPACK_ARCH_DEFINED

#if  ( defined(__aplpha__) || defined(__alpha ) || defined(_M_ALPHA ))
#  define QXPACK_ARCH_ALPHA
#  define QXPACK_ARCH_32BIT

#elif  ( defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64 ) || \
         defined(_M_AMD64 ) || defined(_M_X64))
#  define QXPACK_ARCH_AMD_X64
#  define QXPACK_ARCH_X64
#  define QXPACK_ARCH_X86
#  define QXPACK_ARCH_64BIT

#elif ( defined(i386) || defined(__i386) || defined(__i386__) || defined(__386) || \
      defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__IA32__) || \
      defined(_M_I86)  || defined(_M_IX86) || defined(__X86__) || defined(__I86__) || \
      defined(_THW_INTEL__) || defined(__INTEL__) )
#  define QXPACK_ARCH_X86
#  define QXPACK_ARCH_32BIT

#elif  ( defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || \
        defined(__ARM) || defined(_M_ARM) || defined(_M_ARMT) || defined(__arm) )
#  define QXPACK_ARCH_ARM
#  if defined(__aarch64__)
#    define QXPACK_ARCH_ARM64
#    define QXPACK_ARCH_64BIT
#  else
#    define QXPACK_ARCH_32BIT
#  endif

#elif  ( defined(__aarch64__))
#  define QXPACK_ARCH_ARM64
#  define QXPACK_ARCH_64BIT

#elif  ( defined(__ia64__) || defined(__IA64__) || defined(__ia64) || defined(_IA64)) || \
         defined(_M_IA64)  || defined(__itanium__)
#  define QXPACK_ARCH_IA64
#  define QXPACK_ARCH_64BIT

#elif  ( defined(__mips__) || defined(mips)   || defined(_MIPS_ISA) || defined(__mpis) || \
         defined(_R3000)   || defined(_R4000) || defined(_R5900) || \
         defined(__MIPS__))
#  define QXPACK_ARCH_MIPS
#  define QXPACK_ARCH_32BIT

#elif  ( defined(__powerpc) || defined(__powerpc__) || defined(__ppc) || defiend(_XENON) || \
         defined(__PPCGECKO__) || defined(__PPCBROADWAY__) || defined(_ARCH_PPC) || \
         defined(_M_PPC) || defined(__PPC__) || defined(__POWERPC__))
#  define QXPACK_ARCH_POWERPC
#  if ( defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64))
#    define QXPACK_ARCH_POWERPC64
#    define QXPACK_ARCH_64BIT
#  else
#    define QXPACK_ARCH_32BIT
#  endif

#elif  ( defined(__sparc__) || defined(__sparc) || defined(__sparc_v8__) || defined(__sparc_v9__) || \
         defined(__sparcv8) || defined(__sparcv9))
#  define QXPACK_ARCH_SPARC
#  define QXPACK_ARCH_32BIT
#else
#  define QXPACK_ARCH_UNKNOWN
#endif

#endif

#endif
