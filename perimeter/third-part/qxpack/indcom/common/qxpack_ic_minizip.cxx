#ifndef QXPACK_IC_MINIZIP_CXX
#define QXPACK_IC_MINIZIP_CXX

#include "qxpack_ic_minizip.hxx"
#include "qxpack_ic_logging.hxx"
#include "qxpack_ic_bytearray.hxx"
#include "qxpack_ic_memcntr.hxx"
#include <stdint.h>
#include <cstdlib>
#include <cstring>

// the C style miniz package source
#include <qxpack/indcom/third-part/miniz/miniz.c>
#include <qxpack/indcom/third-part/miniz/miniz_tdef.c>
#include <qxpack/indcom/third-part/miniz/miniz_tinfl.c>

namespace QxPack {

// ////////////////////////////////////////////////////
// local
// ////////////////////////////////////////////////////

// ////////////////////////////////////////////////////
// IcMiniZipComp
// ////////////////////////////////////////////////////
// ====================================================
// CTOR
// ====================================================
    IcMiniZipComp :: IcMiniZipComp ( const char *dp, int dp_sz )
{
    // ------------------------------------------------
    // compress the data
    // ------------------------------------------------
    mz_ulong comp_len = compressBound( mz_ulong( dp_sz ));
    unsigned char *buff = static_cast< unsigned char *>( std::malloc( comp_len ));

    if ( compress( buff, & comp_len, T_PtrCast( const unsigned char *, dp ), mz_ulong( dp_sz ) ) == Z_OK )
    {
        IcByteArray ba( ( int( comp_len + sizeof( uint32_t ))) );
        std::memcpy( ba.data() + sizeof( uint32_t ), buff, comp_len );

        uint32_t  src_sz = uint32_t( dp_sz );
        src_sz = (( src_sz >> 24 ) & 0x00ff ) | (( src_sz >> 8 ) & 0x00ff00 ) |
                 (( src_sz <<  8 ) & 0x00ff0000 ) | (( src_sz << 24 ) & 0xff000000 );
        *( T_PtrCast( uint32_t*, ba.data())) = src_sz;

        m_obj = qxpack_ic_new( IcByteArray, ba );
    } else {
        m_obj = nullptr;
    }
}

// ====================================================
// DTOR
// ====================================================
    IcMiniZipComp :: ~IcMiniZipComp( )
{
    if ( m_obj != nullptr ) { qxpack_ic_delete( m_obj, IcByteArray ); }
}

// ====================================================
// read out the result
// ====================================================
IcByteArray  IcMiniZipComp :: result()
{ return ( m_obj != nullptr ? *( T_ObjCast( IcByteArray*, m_obj )) : IcByteArray());}


// ////////////////////////////////////////////////////
// IcMiniZipDecomp
// /////////////////////////////////////////////////////
// ====================================================
// CTOR
// ====================================================
    IcMiniZipDecomp :: IcMiniZipDecomp ( const char *data, int len )
{
    m_obj = nullptr;
    if ( len < 4 ) { return; }

    uint32_t  src_size = (( uint32_t( data[0] ) << 24 ) & 0xff000000 ) | (( uint32_t( data[1] ) << 16 ) & 0x00ff0000 ) |
                         (( uint32_t( data[2] ) <<  8 ) & 0x00ff00 )   | (( uint32_t( data[3] )) & 0x00ff );
    IcByteArray ba( ( int( src_size ) ) );
    mz_ulong  dst_len = mz_ulong( ba.size());
    if ( uncompress( T_PtrCast( unsigned char *, ba.data()), & dst_len,  T_PtrCast( const unsigned char *, data + sizeof( uint32_t ) ),  mz_ulong( len ) ) == Z_OK ) {
        m_obj = qxpack_ic_new( IcByteArray, ba );
    }
}

// ====================================================
// DTOR
// ====================================================
    IcMiniZipDecomp :: ~IcMiniZipDecomp ( )
{
    if ( m_obj != nullptr ) { qxpack_ic_delete( m_obj, IcByteArray ); }
}

// ====================================================
// result
// ====================================================
IcByteArray  IcMiniZipDecomp :: result()
{ return ( m_obj != nullptr ? *T_ObjCast( IcByteArray *, m_obj ) : IcByteArray()); }


}
#endif
