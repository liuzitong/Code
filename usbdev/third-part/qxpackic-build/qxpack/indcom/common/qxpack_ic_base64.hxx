#ifndef QXPACK_IC_BASE64_HXX
#define QXPACK_IC_BASE64_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_bytearray.hxx"

#if defined( QXPACK_IC_QT_ENABLED )
#include <QString>
#include <QByteArray>
#endif

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// base64 encoder
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  Base64Enc {

public:
    enum  EncAsc { EncAsc_U8 = 0, EncAsc_U16 };

    Base64Enc ( );
    Base64Enc ( const char  *bin, int length, EncAsc enc_asc = EncAsc_U8 );
    Base64Enc ( const Base64Enc & );
    Base64Enc & operator = ( const Base64Enc & );
    virtual ~Base64Enc( );   

    inline IcByteArray    result( ) const { return m_ba; }
    inline IcByteArray&   resultRef( )    { return m_ba; }

    // static functions
    static int    estimEncLength( int src_length );

#if defined( QXPACK_IC_QT_ENABLED )
    static QString    encodeStr  ( const char *src, int src_len );
    static QByteArray encodeByte ( const char *src, int src_len );
#endif

private:
    IcByteArray  m_ba;
};


// ////////////////////////////////////////////////////////////////////////////
//
//  base64 decoder
//
// ////////////////////////////////////////////////////////////////////////////
class  QXPACK_IC_API  Base64Dec {

public:
    Base64Dec ( );
    Base64Dec ( const char     *asc, int length ); // decode from bytes ascii array
    Base64Dec ( const uint16_t *asc, int length ); // decode from UTF16 ascii array
    Base64Dec ( const Base64Dec & );
    Base64Dec & operator = ( const Base64Dec & );  
    virtual ~Base64Dec( );

    inline IcByteArray   result( ) const { return m_ba; }
    inline IcByteArray&  resultRef( )    { return m_ba; }

    static int estimDecLength( int src_length );

#if defined( QXPACK_IC_QT_ENABLED )
    static QByteArray  decode( const QString & src   );
    static QByteArray  decode( const QByteArray &src );
#endif

private:
    IcByteArray m_ba;
};




}

#endif
