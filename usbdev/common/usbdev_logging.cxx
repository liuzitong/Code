// //////////////////////////////////////////////////////////
// @author nightwing
// @history
//        2019/01/14  port from QxPack.indcom
// //////////////////////////////////////////////////////////
#ifndef USBDEV_LOGGING_CXX
#define USBDEV_LOGGING_CXX

#include "usbdev_logging.hxx"
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

namespace UsbDev {

// ////////////////////////////////////////////////
//
// the logging system ( DO NOT Use MemCntr object !)
//
// ////////////////////////////////////////////////
// ===============================================
// ctor
// ===============================================
    Logging :: Logging (const char *file_ptr, int ln )
{
    m_file_ptr = file_ptr; m_ln_num = ln;
}
    Logging :: Logging ( )
{
    m_file_ptr = nullptr; m_ln_num = 0;
}

// ===============================================
// dtor
// ===============================================
    Logging :: ~Logging ( ) { }

// ================================================
// output the fatal message and abort
// ================================================
void   Logging :: fatal ( const char *fmt, ... )
{
    std::time_t t = std::time( nullptr );
    char time_buf[100];
    std::strftime( time_buf, sizeof( time_buf ), "%m/%d/%y %H:%M:%S", std::gmtime(&t) );

    va_list args1;
    va_start( args1, fmt );
    va_list args2;
    va_copy( args2, args1 );
    std::vector<char> buf( size_t( 1 + std::vsnprintf( nullptr, 0, fmt, args1 )) );
    va_end( args1 );
    std::vsnprintf( buf.data(), buf.size(), fmt, args2 );
    va_end( args2 );

    // output the stderr
    std::fprintf( stderr, "[ln] %d in %s\n", m_ln_num, m_file_ptr );
    if ( buf.size() > 1 && buf[ buf.size() - 1 ] == 0x0a ) {
        buf[ buf.size() -1 ] = 0x20;
    }
    std::fprintf( stderr, "%s [fatal]: %s\n", time_buf, buf.data());
    std::fflush ( stderr );
    abort( );
}

// ==============================================
// output the information
// ==============================================
void    Logging :: info ( const char *fmt, ... )
{
    va_list args1;
    va_start( args1, fmt );
    va_list args2;
    va_copy( args2, args1 );
    std::vector<char> buf( size_t( 1 + std::vsnprintf( nullptr, 0, fmt, args1 )) );
    va_end( args1 );
    std::vsnprintf( buf.data(), buf.size(), fmt, args2 );
    va_end( args2 );

    if ( buf.size() > 1 && buf[ buf.size() - 2 ] == 0x0a ) {
        buf[ buf.size() - 2 ] = 0x20;
    }
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
