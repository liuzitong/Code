#ifndef JRPCPLAT_LOGGING_HXX
#define JRPCPLAT_LOGGING_HXX

#include "jrpcplat_def.h"

namespace JRpcPlat {

// ////////////////////////////////////////////////////////////////////////////
//
// simple logging console
//
// ////////////////////////////////////////////////////////////////////////////
class JRPCPLAT_API  Logging {

public:
    Logging ( );
    Logging ( const char *file_ptr, int );
    virtual ~Logging( );
    void  fatal( const char *msg, ... );
    void  info ( const char *msg, ... );
    void  flushInfo( );
private:
    void       *m_rsvd;
    const char *m_file_ptr;
    const char *m_func_ptr;
    int         m_ln_num;
};


}

#define jrpcplat_fatal    JRpcPlat::Logging( __FILE__, __LINE__ ).fatal
#define jrpcplat_info     JRpcPlat:Logging().info
#define jrpcplat_debug       JRpcPlat::Logging( __FILE__, __LINE__ ).info
#define jrpcplat_flush_info  JRpcPlat::Logging().flushInfo

#endif
