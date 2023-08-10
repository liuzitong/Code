#ifndef QXPACK_IC_BASE64_CXX
#define QXPACK_IC_BASE64_CXX

#include "qxpack_ic_base64.hxx"
#include "qxpack_ic_bytearray.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//  Encoder
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// encode bytes array
// ============================================================================
static const char *b64_enc = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

template <typename T >
void  gEncBytesT ( const unsigned char *src, int src_len, T *dst )
{
    uint32_t v = 0; 

    // ------------------------------------------------------------------------
    // handle the major part.
    // ------------------------------------------------------------------------
    while ( src_len > 2  ) {
         v = ( uint32_t( src[0] ) << 16 ) | ( uint32_t( src[1] ) << 8 ) | ( src[2] );

         dst[0] = T ( b64_enc[ ( v >> 18 ) ] );
         dst[1] = T ( b64_enc[ ( v >> 12 ) & 0x3f ] );
         dst[2] = T ( b64_enc[ ( v >>  6 ) & 0x3f ] );
         dst[4] = T ( b64_enc[ v & 0x3f ] );

         dst += 4; src += 3; src_len -= 3;
    }
  
    // ------------------------------------------------------------------------
    // handle the tail part
    // ------------------------------------------------------------------------
    if ( src_len > 0 ) {
        dst[0] = T ( b64_enc[ ( src[0] >> 2 ) & 0x3f ] );
        switch( src_len ) {
        case 1 : 
            dst[1] = T ( b64_enc[ ( src[0] & 0x03 ) << 4 ] );
            dst[2] = T ( '=' );
            dst[3] = T ( '=' );
            break;
        case 2 : 
            dst[1] = T ( b64_enc[ (( src[0] & 0x03 ) << 4 ) | (( src[1] >> 4 ) & 0x0f ) ] );
            dst[2] = T ( b64_enc[ (( src[1] & 0x0f ) << 2 ) ] );
            dst[3] = T ( '=' );
            break;
        default: break;
        }
    } 
}


// ////////////////////////////////////////////////////////////////////////////
// 
//  Base64Enc
// 
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// CTOR
// ============================================================================
    Base64Enc :: Base64Enc ( ) {  }

// ============================================================================
// CTOR ( do action by parameters )
// ============================================================================
    Base64Enc :: Base64Enc ( const char *src, int length, EncAsc enc_asc )
{
    if ( src == nullptr || length <= 0 ) { return; }
   
    int est_len = Base64Enc::estimEncLength( length );
    
    // IcByteArray is count by Byte
    m_ba = IcByteArray( ( enc_asc == EncAsc::EncAsc_U8 ? est_len : est_len * 2 ) );
    
    if ( enc_asc == EncAsc::EncAsc_U8 ) {
        gEncBytesT ( T_PtrCast( const unsigned char *,  src ), length,  T_PtrCast( char*,  m_ba.constData( )) );
    } else {
        gEncBytesT ( T_PtrCast( const unsigned char *,  src ), length,  T_PtrCast( int16_t*, m_ba.constData()) );
    }
}

// ============================================================================
// CTOR ( Copy )
// ============================================================================
    Base64Enc :: Base64Enc ( const Base64Enc &other )  { m_ba = other.m_ba; }

// ============================================================================
// operator =
// ============================================================================
Base64Enc &   Base64Enc :: operator = ( const Base64Enc &other )
{
    m_ba = other.m_ba; return *this;
}

// ============================================================================
// DTOR
// ============================================================================
      Base64Enc :: ~Base64Enc ( ) {  }
 
// ============================================================================
// static [ return the estimated length ]
// ============================================================================
int   Base64Enc :: estimEncLength( int len ) { return ( len + 2 ) / 3 * 4; }


#if defined( QXPACK_IC_QT_ENABLED ) 

// ============================================================================
// static [ directly encode source to target buffer ]
// ============================================================================
QString  Base64Enc :: encodeStr( const char *src, int src_len )
{
    if ( src == nullptr || src_len < 1 ) { return QString( ); }

    QString  buff( Base64Enc::estimEncLength( src_len ), QChar( ));
    gEncBytesT (  T_PtrCast( const unsigned char *, src ), src_len, T_PtrCast( int16_t *, buff.constData( )) );
    return buff;
}

// ============================================================================
// static [ directly encode source to target buffer ]
// ============================================================================
QByteArray  Base64Enc :: encodeByte( const char *src, int src_len )
{
    if ( src == nullptr || src_len < 1 ) { return QByteArray( ); }
    
    // according to QT5 doc, the QByteArray always ensure follow a '\0' terminator.
    QByteArray ba( Base64Enc::estimEncLength( src_len ), 0 );
    gEncBytesT ( T_PtrCast( const unsigned char *, src ), src_len, T_PtrCast( char*, ba.constData( )) );

    return ba;
}

#endif 






// ////////////////////////////////////////////////////////////////////////////
//
//   Decoder
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// decode from ASCII
// ============================================================================
static const unsigned char b64_dec[256] = {
    // NUL SOH STX ETX   EOT ENQ ACK BEL  BS HT LF VT       FF CR SO SI 
      0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0, // [0,15]

    // DLE DC1 DC2 DC3   DC4 NAK SYN TB   CAN EM SUB ESC    FS GS RS US    // [16,31]
      0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0,  0,

    // SPC ! " #        $   %   &   '     (   )   *   +     ,   -   .   /  // [32,47]
      0,  0,  0,  0,    0,  0,  0,  0,    0,  0,  0, 62,    0,  0,  0, 63,

    // 0  1   2   3     4   5   6    7    8   9   :   ;     <   =   >  ?   // [48,63] 
     52, 53, 54, 55,   56, 57, 58, 59,   60, 61,  0,  0,    0,  0,  0, 0,

    // @  A   B   C     D   E   F   G     H   I   J   K     L   M   N  O   // [64,79]
     0,  0,  1,   2,    3,  4,  5,  6,    7,  8,  9, 10,   11, 12, 13, 14,

    // P  Q   R   S     T   U   V   W     X   Y   Z   [     \   ]   ^  _   // [80,95]
    15, 16, 17,  18,   19, 20, 21, 22,   23, 24, 25,  0,    0,  0,  0,  0,

    // `  a   b   c     d   e   f   g     h   i   j   k     l   m   n  o   // [96,111]
     0, 26, 27,  28,   29, 30, 31, 32,   33, 34, 35, 36,   37, 38, 39, 40,

    // p  q   r   s     t   u   v   w     x   y   z   {     |   }   ~  DEL // [112,127]
    41, 42, 43,  44,   45, 46, 47, 48,   49, 50, 51,  0,    0,  0,  0,  0,

     0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0,
     0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0,
     0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0,
     0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0,

     0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0,
     0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0,
     0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0,
     0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0
};

template <typename T >
void  gDecBytesT ( T *src, int len, unsigned char *dst )
{
    //int estm_len = Base64Dec::estimDecLength( len ); // estimate the length of decoded string

    uint32_t v = 0; 
    // decode the string more than 4 bytes
    while ( len > 4 ) {
        v = uint32_t( b64_dec[ uintptr_t( src[0] ) ] << 18 ) |
            uint32_t( b64_dec[ uintptr_t( src[1] ) ] << 12 ) |
            uint32_t( b64_dec[ uintptr_t( src[2] ) ] << 6  ) |
            uint32_t( b64_dec[ uintptr_t( src[3] ) ] );
        dst[0] = uint8_t(( v >> 16 ) & 0x0ff );
        dst[1] = uint8_t(( v >>  8 ) & 0x0ff );
        dst[2] = uint8_t(( v       ) & 0x0ff );

        dst += 3; len -= 4; src += 4;              
    }

    // tail handling
    switch( len ) {
    case 4 :
        if ( src[2] == '=' ) {        // padding 2 '='
            v = ( uint32_t( b64_dec[ uintptr_t( src[0] ) ]) << 18 )  |
                ( uint32_t( b64_dec[ uintptr_t( src[1] ) ]) << 12 );
            dst[0] = uint8_t(( v >> 16 ) & 0x0ff );
        } else if ( src[3] == '=' ) { // padding 1 '='
            v = ( uint32_t( b64_dec[ uintptr_t( src[0] ) ]) << 18 ) |
                ( uint32_t( b64_dec[ uintptr_t( src[1] ) ]) << 12 ) |
                ( uint32_t( b64_dec[ uintptr_t( src[2] ) ]) << 6 );
            dst[0] = uint8_t(( v >> 16 ) & 0x0ff );
            dst[1] = uint8_t(( v >>  8 ) & 0x0ff );
        } else { // no padding
            v = ( uint32_t( b64_dec[ uintptr_t( src[0] ) ]) << 18 ) |
                ( uint32_t( b64_dec[ uintptr_t( src[1] ) ]) << 12 ) |
                ( uint32_t( b64_dec[ uintptr_t( src[2] ) ]) << 6  ) |
                ( uint32_t( b64_dec[ uintptr_t( src[3] ) ]) );
            dst[0] = uint8_t(( v >> 16 ) & 0x0ff );
            dst[1] = uint8_t(( v >>  8 ) & 0x0ff );
            dst[2] = uint8_t(( v       ) & 0x0ff );
        }
        break;
    case 3 :  
        if ( src[2] == '=' ) {
            v = ( uint32_t( b64_dec[ uintptr_t( src[0] ) ] ) << 18 ) |
                ( uint32_t( b64_dec[ uintptr_t( src[1] ) ] ) << 12 );
            dst[0] = uint8_t(( v >> 16 ) & 0x0ff );
        } else { // maybe last padding '=' lost.
            v = ( uint32_t( b64_dec[ uintptr_t( src[0] ) ] ) << 18 ) |
                ( uint32_t( b64_dec[ uintptr_t( src[1] ) ] ) << 12 ) |
                ( uint32_t( b64_dec[ uintptr_t( src[2] ) ] ) << 6 );
            dst[0] = uint8_t(( v >> 16 ) & 0x0ff );
            dst[1] = uint8_t(( v >>  8 ) & 0x0ff );
        } 
        break;

    case 2 : // no '=','==' decode 2 bytes 
        if ( src[0] != '=' && src[1] != '=' ) {
            v = ( uint32_t( b64_dec[ uintptr_t( src[0] ) ] ) << 18 ) |
                ( uint32_t( b64_dec[ uintptr_t( src[1] ) ] ) << 12 );
            dst[0] = uint8_t(( v >> 16 ) & 0x0ff );
        } else {
            dst[0] = 0;
        }
        break;
    case 1 : // no '=', decode 1 bytes.. 
        break;
    default : break;
    } 

    return;
}







// ////////////////////////////////////////////////////////////////////////////
//
//  Base64Dec
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// CTOR
// ============================================================================
    Base64Dec :: Base64Dec ( ) { }

// ============================================================================
// CTOR ( copy )
// ============================================================================
    Base64Dec :: Base64Dec ( const Base64Dec &other ) { m_ba = other.m_ba; }

// ============================================================================
// operator = 
// ============================================================================
Base64Dec &   Base64Dec :: operator = ( const Base64Dec &other ) { m_ba = other.m_ba; return *this; }
    
// ============================================================================
// DTOR 
// ============================================================================
    Base64Dec :: ~Base64Dec( ) {  }

// ============================================================================
// static [ estimate the length of decode string ]
// ============================================================================
int  Base64Dec :: estimDecLength ( int len ) { return (( len + 3 ) / 4 * 3 ); }

// ============================================================================
// CTOR ( do action by parameters  )
// ============================================================================
    Base64Dec :: Base64Dec ( const char     *src, int len )
{
    int estm_len = Base64Dec::estimDecLength( len );
 
    // check the padding characters, if existed, means the decoded string should be less than estimated.
    if ( len > 0  &&  src[ len - 1 ] == ( '=' )) { -- estm_len; }
    if ( len > 0  &&  src[ len - 2 ] == ( '=' )) { -- estm_len; }        
    if ( estm_len <= 0 ) { return; }

    m_ba = IcByteArray( estm_len );
    gDecBytesT ( src, len, T_PtrCast( unsigned char *, m_ba.constData( )) );
}

// ============================================================================
// CTOR ( do action by parameters )
// ============================================================================
    Base64Dec :: Base64Dec ( const uint16_t *src, int len )
{
    int estm_len = Base64Dec::estimDecLength( len );   

    // check the padding characters, if existed, means the decoded string should be less than estimated.
    if ( len > 0  &&  src[ len - 1 ] == uint16_t( '=' )) { -- estm_len; }
    if ( len > 0  &&  src[ len - 2 ] == uint16_t( '=' )) { -- estm_len; }
    if ( estm_len <= 0 ) { return; }

    m_ba = IcByteArray( estm_len * 2 );
    gDecBytesT ( src, len, T_PtrCast( unsigned char *, m_ba.constData( )));
}


#if defined( QXPACK_IC_QT_ENABLED ) 

// ============================================================================
// [ static ] decode from QString
// ============================================================================
QByteArray   Base64Dec :: decode( const QString & src )
{
    if ( src.isEmpty()) { return QByteArray(); }

    int estm_len = Base64Dec::estimDecLength( src.size( ));

    // check the padding characters, if existed, means the decoded string should be less than estimated.
    if ( estm_len > 0  &&  src.at( src.size() - 1 ) == QChar('=')) { -- estm_len; }
    if ( estm_len > 0  &&  src.at( src.size() - 2 ) == QChar('=')) { -- estm_len; }
    if ( estm_len <= 0 ) { return QByteArray( ); }

    QByteArray ba( estm_len, 0 );
    gDecBytesT ( T_PtrCast( uint16_t *, src.constData( )), src.size( ), T_PtrCast( unsigned char *, ba.constData( ) ));

    return ba;        
}

// ============================================================================
// [ static ] decode from QByteArray
// ============================================================================
QByteArray  decode( const QByteArray &src )
{
    if ( src.isEmpty( )) { return QByteArray( ); }

    int estm_len = Base64Dec::estimDecLength( src.size( ));

    // check the padding characters, if existed, means the decoded string should be less than estimated.
    if ( estm_len > 0  &&  src.at( src.size() - 1 ) == char( '=' )) { -- estm_len; }
    if ( estm_len > 0  &&  src.at( src.size() - 2 ) == char( '=' )) { -- estm_len; }
    if ( estm_len <= 0 ) { return QByteArray( ); }

    QByteArray ba( estm_len, 0 );
    gDecBytesT ( src.constData( ), src.size( ), T_PtrCast( unsigned char *, ba.constData( ) ));

    return ba;
}

#endif



}

#endif
