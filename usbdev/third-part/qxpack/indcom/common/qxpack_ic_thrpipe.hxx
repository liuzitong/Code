#ifndef QXPACK_IC_THRPIPE_HXX
#define QXPACK_IC_THRPIPE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_variant.hxx"
#include <cstdint>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// the thr pipe package used to describe user command
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcThrPipePkg QXPACK_IC_FINAL {
public:
    explicit IcThrPipePkg( );
    ~IcThrPipePkg( );
    IcThrPipePkg( const IcThrPipePkg & );
    IcThrPipePkg & operator = ( const IcThrPipePkg & );

#if defined( QXPACK_IC_CXX11 )
    IcThrPipePkg ( IcThrPipePkg && );
    IcThrPipePkg & operator = ( IcThrPipePkg && );
#endif

    int64_t  id() const;
    void     setId( const int64_t& );

    IcVariant  var() const;
    void  setVar( const IcVariant & );

private:
    void *m_obj;
};


// ////////////////////////////////////////////////////////////////////////////
//
// the thr pipe used to pass command to another thread [ thread ]
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcThrPipe QXPACK_IC_FINAL {
public:
    explicit IcThrPipe( );

    ~IcThrPipe( );

    bool  post(   );



private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcThrPipe )
};

}

#endif
