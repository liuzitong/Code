#ifndef QXPACK_IC_MINIZIP_HXX
#define QXPACK_IC_MINIZIP_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_bytearray.hxx"

namespace QxPack {

// /////////////////////////////////////////////
//
// MiniZip compress
//
// //////////////////////////////////////////////
class QXPACK_IC_API IcMiniZipComp {
public:
    IcMiniZipComp( const char *data, int len );
    virtual ~IcMiniZipComp( );
    IcByteArray  result( );
private:
    void *m_obj;
    IcMiniZipComp( const IcMiniZipComp & );
    IcMiniZipComp & operator = ( const IcMiniZipComp & );
};

// //////////////////////////////////////////////
//
//  MiniZip decompress
//
// //////////////////////////////////////////////
class QXPACK_IC_API  IcMiniZipDecomp {
public:
    IcMiniZipDecomp( const char *data, int len );
    virtual ~IcMiniZipDecomp( );
    IcByteArray   result( );
private:
    void *m_obj;
    IcMiniZipDecomp( const IcMiniZipDecomp & );
    IcMiniZipDecomp & operator = ( const IcMiniZipDecomp & );
};




}
#endif
