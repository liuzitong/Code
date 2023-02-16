#ifndef PERIMETER_LOGGING_HXX
#define PERIMETER_LOGGING_HXX

#include "perimeter_def.h"

namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
//
// simple logging console
//
// ////////////////////////////////////////////////////////////////////////////
class PERIMETER_API  Logging {

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

#define perimeter_fatal    Perimeter::Logging( __FILE__, __LINE__ ).fatal
#define perimeter_info     Perimeter:Logging().info
#define perimeter_debug       Perimeter::Logging( __FILE__, __LINE__ ).info
#define perimeter_flush_info  Perimeter::Logging().flushInfo

#endif
