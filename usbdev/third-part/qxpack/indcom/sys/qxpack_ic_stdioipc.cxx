#ifndef QXPACK_IC_STDIOIPC_CXX
#define QXPACK_IC_STDIOIPC_CXX

#include "qxpack_ic_stdioipc.hxx"
#include <qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp>
#include <memory>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <cstring>
#include <memory>
#include <stdint.h>

#define ASCII_ESC      0x1b
#define ASCII_ESC_HEAD 0x80

#if defined( QXPACK_IC_QT_ENABLED )
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#endif

namespace QxPack {

// ////////////////////////////////////////////
// local functions
// ////////////////////////////////////////////
// =============================================
// get the number of the text string
// =============================================
static int  gNumFromText( const unsigned char *text, int len )
{
    int num = 0;
    while ( len -- > 0 ) {
        unsigned char chr = *text ++;
        if ( chr >= 0x30 && chr <= 0x39 ) {
            num = num * 10 + ( chr - 0x30 );
        } else {
            break;
        }
    }
    return num;
}

// ==============================================
//  generate the number to text
// ==============================================
static void gGenNumText( unsigned char *text, int len, int num )
{
    unsigned char *t = text;

    // fill as blank character
    std::memset( text, 0x20, size_t( len ));

    // generate the number string of num
    int n_cntr = 10;
    while ( num > 0 && len -- > 0 && n_cntr -- > 0 ) {
        *t ++ = ( uint8_t( num % 10 ) + 0x30 );
        num /= 10;
    }

    // rev. the text string
    -- t; unsigned char t_chr;
    while ( t > text ) {
        t_chr = *text; *text = *t; *t = t_chr;
        ++ text; -- t;
    }
}

// ////////////////////////////////////////////
//
// IcStdioIpc wrapper
//
// ////////////////////////////////////////////
// ============================================
// CTOR
// ============================================
    IcStdioIpc :: IcStdioIpc( ) { m_obj = nullptr; }

// ============================================
// DTOR
// ============================================
    IcStdioIpc :: ~IcStdioIpc( ) { }

// ============================================
// try read the record data from stdin,
// NOTE: the record string is below:
//     0x1b, 0x80, ( 10 ASCII number of length. BE order ) +
// stream length..
// ============================================
IcByteArray  IcStdioIpc :: readRec( bool *is_ok  )
{
    IcByteArray ba_in;
    int text_size = INT_MAX; int read_num = -1;

    // ----------------------------------------------
    // here we try to read the record from stdin
    // ----------------------------------------------
    while ( std::feof( stdin ) == 0 && std::ferror( stdin ) == 0 ) {
        unsigned char buff[ 16 ];

        // --------------------------------------
        // try read until the header magic sign
        // --------------------------------------
        read_num = int( std::fread( buff, 1, 1, stdin ));
        if ( read_num < 1 ) { read_num = -1; continue; }   // EOF or an error
        if ( buff[0] != uint8_t( ASCII_ESC )) { read_num = -1; continue; }      // need read more.
        read_num = int( std::fread( buff, 1, 1, stdin ));
        if ( read_num < 1 ) { read_num = -1; continue; }
        if ( buff[0] != uint8_t( 0x80 )) { read_num = -1; continue; }           // ESC after is not 0x80. bad..

        // ----------------------------------------
        // parse the size header, here do a loop read 10 characters
        // ----------------------------------------
        read_num = 0;
        while ( std::feof( stdin ) == 0 && std::ferror( stdin ) == 0 &&
                read_num < 10
              )
        {
            read_num += int( std::fread( buff + read_num, 1, size_t( 10 - read_num ), stdin ));
        }
        if ( read_num < 10 ) { read_num = -1; continue; }  // not enough text..
        text_size = gNumFromText( buff, 10 );
        if ( text_size < 1 ) { break; }   // zero data, break

        // ------------------------------------------------
        // read the text stream into byte array object
        // NOTE: here need to do a loop due to pipe buffer.
        // ------------------------------------------------
        ba_in.resize( text_size ); read_num = 0;
        while ( std::feof( stdin ) == 0 && std::ferror( stdin ) == 0 &&
                read_num < text_size
              )
        {
            read_num += int(std::fread( ba_in.data() + read_num,
                                    1, size_t( text_size - read_num ), stdin
                                  ));
        }
        break;
    }

    // --------------------------------------
    // check the result we got
    // --------------------------------------
    if ( is_ok != nullptr ) {
        *is_ok = true;
        // cond. EOF && read size less than req.
        if ( read_num < text_size ) { *is_ok = false; ba_in = IcByteArray(); }
        if ( std::feof( stdin )   != 0  &&  read_num < text_size )
        { *is_ok = false; ba_in = IcByteArray(); }
        if ( std::ferror( stdin ) != 0 )
        { *is_ok = false; ba_in = IcByteArray(); }
    }

    return ba_in;
}

// ====================================================
// write the data as record
// ====================================================
bool      IcStdioIpc :: writeRec( const char *data, int data_size )
{
    if ( data == nullptr || data_size <= 0 ) { return true; }

    unsigned char header[12] = { 0x1b, 0x80 };
    gGenNumText( & header[2], 10, data_size );

    if ( std::fwrite( header, 1, sizeof( header ), stdout ) != sizeof( header ) )
    { return false; }
    if ( std::fwrite( data, 1, size_t( data_size ), stdout ) != size_t( data_size ))
    { return false; }

    return ( std::fflush( stdout ) == 0 );
}

// ======================================================
// read from stdin, use normal method ( means read until ctrl+z )
// ======================================================
IcByteArray  IcStdioIpc :: readAll ( int block_size, bool *is_ok )
{
    std::list<IcByteArray>  ba_list; int total_bytes = 0;
    if ( block_size <= 8 ) { block_size = 8; }

    // read string data until EOF of Ctrl + z
    while ( std::feof( stdin ) == 0 && std::ferror( stdin ) == 0 ) {
        IcByteArray ba( block_size + int( sizeof( int )) ); // use header data as vaild data size.
        int read_num = int( std::fread( ba.data() + sizeof( int ), 1, size_t( block_size ), stdin ));
        if ( read_num < 1 ) { continue; }
        *( T_PtrCast( int*, ba.data()) ) = read_num; total_bytes += read_num;
        ba_list.push_back( ba );
    }

    // check if current read all content
    if ( ba_list.size() <= 0 ) {
        if ( is_ok != nullptr ) { *is_ok = false; }
        return IcByteArray();
    }

    // combin all data into one block
    IcByteArray ret_ba( total_bytes ); int offset = 0;
    while ( ! ba_list.empty() ) { // nw: 2019/05/10 old is -- ( ba_list.size() > 0 ) {
        IcByteArray ba = ba_list.front(); ba_list.pop_front();
        int  valid_bytes = *( T_PtrCast( int*, ba.constData()) );
        if ( valid_bytes <= 0 ) { continue; }                   // bad ?
        if ( offset + valid_bytes > total_bytes ) { continue; } // overflow ?

        std::memcpy( ret_ba.data() + offset, ba.data() + sizeof( int ), size_t( valid_bytes ));
        offset += valid_bytes;
    }

    if ( is_ok != nullptr ) { *is_ok = true; }
    return ret_ba;
}

// =====================================================
//  write the data into stdout
// =====================================================
bool         IcStdioIpc :: write( const char *data, int data_size )
{
    if ( std::fwrite( data, 1, size_t( data_size ), stdout ) != size_t( data_size ))
    { return false; }
    return ( std::fflush( stdout ) == 0 );
}

// =====================================================
// check if the stdin closed [ static ]
// =====================================================
bool     IcStdioIpc :: isStdInClosed()
{
    return ( std::feof( stdin ) != 0 );
}

} // namespace end






#if defined( QXPACK_IC_QT_ENABLED )

#include <QProcess>
#include <QDebug>

namespace QxPack {

// ///////////////////////////////////////////////////
//
//  IcQProcStdioIpc ( used for QT5 QProcess )
//
// ///////////////////////////////////////////////////
#define T_ProcPtr( o )  T_ObjCast( QProcess*, o )
// ===================================================
// CTOR
// ===================================================
    IcQProcStdioIpc :: IcQProcStdioIpc ( QProcess &proc )
{
    m_obj = &proc;
    QObject::connect( & proc, SIGNAL( destroyed( QObject* )), this, SLOT( qprocess_onDestroyed( QObject*)));
}

// ====================================================
// DTOR
// ====================================================
    IcQProcStdioIpc :: ~IcQProcStdioIpc ( )
{
    if ( m_obj != nullptr ) {
        QObject::disconnect( T_ProcPtr( m_obj ), Q_NULLPTR, this, Q_NULLPTR );
    }
}

// ====================================================
// SLOT: handle the QProcess destroyed
// ====================================================
void  IcQProcStdioIpc :: qprocess_onDestroyed( QObject *obj )
{
    if ( obj == qobject_cast< QObject*>( T_ProcPtr( m_obj ))) {
        m_obj = nullptr;
    }
}

// =====================================================
// read the record from process output
// =====================================================
IcByteArray  IcQProcStdioIpc :: readRec ( bool *is_ok )
{
    IcByteArray ba_in;
    qint64 text_size = INT_MAX; qint64 read_num = -1;

    // ------------------------------------
    // check if QProcess is valid
    // ------------------------------------
    if ( m_obj == nullptr ) { 
        if ( is_ok != nullptr ) { *is_ok = false; }
        return ba_in;
    }

    // -------------------------------------
    // read from process
    // -------------------------------------
    QProcess &proc = * T_ProcPtr( m_obj );
    bool is_req_wait_for_read = true;
    while ( proc.state() == QProcess::Running  &&  proc.isReadable() )
    {
        // ---------------------------------
        // wait for byte incoming
        // ---------------------------------
        if ( is_req_wait_for_read && ! proc.waitForReadyRead( ))
        { is_req_wait_for_read = false; break; }

        // ---------------------------------
        // read the header...
        // ---------------------------------
        unsigned char buff[16];
        read_num = proc.read( T_PtrCast( char*, buff ), 1 );
        if ( read_num == 0 ) { is_req_wait_for_read = true; read_num = -1; continue; }
        if ( read_num <  0 ) {  break; } // bad reading
        if ( buff[0] != uint8_t( ASCII_ESC )) { read_num = -1; continue; } // try read next character.

        read_num = proc.read( T_PtrCast( char*, buff ), 1 );
        if ( read_num == 0 ) { is_req_wait_for_read = true; read_num = -1; continue; }
        if ( read_num <  0 ) { break; } // bad reading
        if ( buff[0] != uint8_t( 0x80 )) { read_num = -1; continue; }

        // ----------------------------------
        // read the text size
        // ----------------------------------
        read_num = 0;
        while( proc.state() == QProcess::Running && read_num < 10 )
        {
            qint64 curr_read = proc.read( T_PtrCast( char*, buff ) + read_num, 10 - read_num );
            if ( curr_read == 0 ) { proc.waitForReadyRead(); }
            if ( curr_read <  0 ) { break; }
            read_num += curr_read;
        }
        if ( read_num < 10 ) { read_num = -1; continue; }
        text_size = qint64( gNumFromText( buff, 10 ));
        if ( text_size < 1 ) { read_num = -1; continue; }

        // ----------------------------------------
        // read the data into byte array
        // ----------------------------------------
        ba_in.resize( int( text_size )); read_num = 0;
        while ( proc.state() == QProcess::Running && read_num < text_size )
        {
            qint64 curr_read =  proc.read( T_PtrCast( char*, ba_in.data() ) + read_num,
                                           text_size - read_num
                                          );
            if ( curr_read == 0 ) { proc.waitForReadyRead(); }
            if ( curr_read <  0 ) { break; }
            read_num += curr_read;
        }
        break;
    }

    // check if the ba_in is not empty?
    if ( is_ok != nullptr ) {
        *is_ok = true;
        if ( read_num < text_size ) { *is_ok = false; }
    }

    return ba_in;
}

// ======================================================
// write the record to process input
// ======================================================
bool     IcQProcStdioIpc :: writeRec( const char *data, int data_size )
{
    if ( m_obj == nullptr ) { return false; }
    if ( data == nullptr || data_size <= 0 ) { return false; }
    QProcess &proc = * T_ProcPtr( m_obj );
    if ( proc.state() != QProcess::Running ) { return false; }
    if ( ! proc.isWritable() ) { return false; }

    unsigned char header[12] = { 0x1b, 0x80 };
    gGenNumText( & header[2], 10, data_size );

    if( proc.write( T_PtrCast( const char *, header ), sizeof( header )) != sizeof( header )) { return false; }
    if( proc.write( data, data_size ) != data_size ) { return false; }

    return proc.waitForBytesWritten( );
}

// ==============================================================
// read from the target output
// ==============================================================
IcByteArray  IcQProcStdioIpc::read ( int block_size, bool *is_ok )
{
    std::list<IcByteArray>  ba_list; int total_bytes = 0;
    if ( block_size <= 8 ) { block_size = 8; }

    // -------------------------------------------------
    // reading
    // -------------------------------------------------
    QProcess &proc = * T_ProcPtr( m_obj );
    bool is_req_wait_for_read = true;
    while ( proc.state() == QProcess::Running  &&  proc.isReadable() ) {
        if ( is_req_wait_for_read && ! proc.waitForReadyRead( ))
        { is_req_wait_for_read = false; break; }

        // nw: 2018/05/31 detect how many bytes need to read.
        qint64 bytes_count = proc.bytesAvailable();
        if ( bytes_count < 1 ) { continue; }

        // nw: 2018/05/31 read all bytes count from buffer
        while ( bytes_count > 0 ) {
            IcByteArray ba( block_size + int( sizeof( int )) ); // use header data as vaild data size.
            qint64 read_num = proc.read( ba.data() + sizeof( int ), block_size );
            if ( read_num < 1 ) { break; }
            *( T_PtrCast( int*, ba.data()) ) = int( read_num );
            total_bytes += int( read_num );
            bytes_count -= int( read_num );
            ba_list.push_back( ba );
        }
    }

    // --------------------------------------------------
    // check if current read all content
    // --------------------------------------------------
    if ( ba_list.size() <= 0 ) {
        if ( is_ok != nullptr ) { *is_ok = false; }
        return IcByteArray();
    }

    // -------------------------------------------------
    // combin all data into one block
    // ------------------------------------------------
    IcByteArray ret_ba( total_bytes ); int offset = 0;
    while ( ! ba_list.empty() ) { // nw: 2019/05/10 old is -- ( ba_list.size() > 0 ) {
        IcByteArray ba = ba_list.front(); ba_list.pop_front();
        int  valid_bytes = *( T_PtrCast( int*, ba.constData()) );
        if ( valid_bytes <= 0 ) { continue; }                   // bad ?
        if ( offset + valid_bytes > total_bytes ) { continue; } // overflow ?

        std::memcpy( ret_ba.data() + offset, ba.data() + sizeof( int ), size_t( valid_bytes ));
        offset += valid_bytes;
    }

    if ( is_ok != nullptr ) { *is_ok = true; }
    return ret_ba;
}

// ============================================================
// write the data
// ============================================================
bool       IcQProcStdioIpc :: write( const char *data, int data_size, bool close_after_write )
{
    if ( m_obj == nullptr ) { return false; }
    if ( data == nullptr || data_size <= 0 ) { return false; }
    QProcess &proc = * T_ProcPtr( m_obj );
    if ( proc.state() != QProcess::Running ) { return false; }
    if ( ! proc.isWritable() ) { return false; }
    if( proc.write( data, data_size ) != data_size ) { return false; }
    bool ret = proc.waitForBytesWritten( );
    if ( close_after_write ) { proc.closeWriteChannel(); }
    return ret;
}



}

#endif




#endif
