// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @history
//        2018/12/20  modified, for GCC 5 compiler
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_LOGGING_CXX
#define QXPACK_IC_LOGGING_CXX

#include "qxpack_ic_logging.hxx"
#include <stddef.h>
#include <cstdio>  // c++11 req.
#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include <vector>

#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#endif

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// the logging system ( DO NOT Use MemCntr object !)
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
    IcLogging :: IcLogging (const char *file_ptr, int ln )
{
    m_file_ptr = file_ptr; m_ln_num = ln;
}
    IcLogging :: IcLogging ( )
{
    m_file_ptr = nullptr; m_ln_num = 0;
}

// ============================================================================
// dtor
// ============================================================================
    IcLogging :: ~IcLogging ( ) { }

// ============================================================================
// output the fatal message and abort
// ============================================================================
void   IcLogging :: fatal ( const char *fmt, ... )
{
    std::time_t t = std::time( nullptr );
    char time_buf[100];
    std::strftime( time_buf, sizeof( time_buf ), "%m/%d/%y %H:%M:%S", std::gmtime(&t) );

    va_list args1;
    va_start( args1, fmt );
    int vect_sz = 1024; std::vector<char> buf; buf.reserve(vect_sz);
    while ( std::vsprintf( buf.data(), fmt, args1 ) < 0 ) {
        buf = std::vector<char>( ( vect_sz += 1024 ));
    }
    va_end( args1 );

    std::fprintf( stderr, "[ln] %d in %s\n", m_ln_num, m_file_ptr );
    std::fprintf( stderr, "%s [fatal]: %s\n", time_buf, buf.data());
    std::fflush ( stderr );
    abort( );
}

// ============================================================================
// output the information
// ============================================================================
void    IcLogging :: info ( const char *fmt, ... )
{   
    va_list args1;
    va_start( args1, fmt );
    int vect_sz = 1024; std::vector<char> buf; buf.reserve(vect_sz);
    while ( std::vsprintf( buf.data(), fmt, args1 ) < 0 ) {
        buf = std::vector<char>( ( vect_sz += 1024 ));
    }
    va_end( args1 );

    std::fprintf( stdout, "[info]: %s\n", buf.data());
}

void    IcLogging :: flushInfo()
{
    std::fflush( stdout );
}

}


#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
