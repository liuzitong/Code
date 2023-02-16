// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @history
//        2019/01/14  port from QxPack.indcom
// ////////////////////////////////////////////////////////////////////////////
#ifndef PERIMETER_LOGGING_CXX
#define PERIMETER_LOGGING_CXX

#include "perimeter_logging.hxx"
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

namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
//
// the logging system ( DO NOT Use MemCntr object !)
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
    Logging :: Logging (const char *file_ptr, int ln )
{
    m_file_ptr = file_ptr; m_ln_num = ln;
}
    Logging :: Logging ( )
{
    m_file_ptr = nullptr; m_ln_num = 0;
}

// ============================================================================
// dtor
// ============================================================================
    Logging :: ~Logging ( ) { }

// ============================================================================
// output the fatal message and abort
// ============================================================================
void   Logging :: fatal ( const char *fmt, ... )
{
    std::time_t t = std::time( nullptr );
    char time_buf[100];
    std::strftime( time_buf, sizeof( time_buf ), "%m/%d/%y %H:%M:%S", std::gmtime(&t) );

    va_list args1;
    va_start( args1, fmt );
    int vect_sz = 0; std::vector<char> buf;
    do {
        buf = std::vector<char>((vect_sz += 1024 ));
    } while ( std::vsprintf( buf.data(), fmt, args1 ) < 0 );
    va_end( args1 );

    std::fprintf( stderr, "[ln] %d in %s\n", m_ln_num, m_file_ptr );
    std::fprintf( stderr, "%s [fatal]: %s\n", time_buf, buf.data());
    std::fflush ( stderr );
    abort( );
}

// ============================================================================
// output the information
// ============================================================================
void    Logging :: info ( const char *fmt, ... )
{
    va_list args1;
    va_start( args1, fmt );
    int vect_sz = 0; std::vector<char> buf;
    do {
        buf = std::vector<char>(( vect_sz += 1024 ));
    } while ( std::vsprintf( buf.data(), fmt, args1 ) < 0 );
    va_end( args1 );

    std::fprintf( stdout, "[info]: %s\n", buf.data());
}

void    Logging :: flushInfo()
{
    std::fflush( stdout );
}

}


#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
