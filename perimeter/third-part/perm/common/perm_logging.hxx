#ifndef PERM_LOGGING_HXX
#define PERM_LOGGING_HXX

#include "perm_def.h"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//
// simple logging console
//
// ////////////////////////////////////////////////////////////////////////////
class PERM_API  Logging {

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

#define perm_fatal    FcPerm::Logging( __FILE__, __LINE__ ).fatal
#define perm_info     FcPerm:Logging().info
#define perm_debug       FcPerm::Logging( __FILE__, __LINE__ ).info
#define perm_flush_info  FcPerm::Logging().flushInfo

#endif
