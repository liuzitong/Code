#ifndef USBDEV_LOGGING_HXX
#define USBDEV_LOGGING_HXX

#include "usbdev_def.h"

namespace UsbDev {

// ////////////////////////////////////////////////////////////////////////////
//
// simple logging console
//
// ////////////////////////////////////////////////////////////////////////////
class USBDEV_API  Logging {

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

#define usbdev_fatal    UsbDev::Logging( __FILE__, __LINE__ ).fatal
#define usbdev_info     UsbDev:Logging().info
#define usbdev_debug       UsbDev::Logging( __FILE__, __LINE__ ).info
#define usbdev_flush_info  UsbDev::Logging().flushInfo

#endif
