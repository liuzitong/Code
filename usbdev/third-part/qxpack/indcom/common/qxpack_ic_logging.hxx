#ifndef QXPACK_IC_LOGGING_HXX
#define QXPACK_IC_LOGGING_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#ifdef QXPACK_IC_QT_ENABLED
#  include <QDebug>
#endif


namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// simple logging console
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcLogging {

public:
    IcLogging ( );
    IcLogging ( const char *file_ptr, int );
    virtual ~IcLogging( );
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

#if defined( QXPACK_IC_QT_ENABLED )

#define qxpack_ic_fatal   qFatal
#define qxpack_ic_info    qInfo
#define qxpack_ic_debug   qDebug
#define qxpack_ic_flush_info
#define qxpack_ic_warning  qWarning
#define qxpack_ic_critical qCritical

#else

#define qxpack_ic_fatal       QxPack::IcLogging( __FILE__, __LINE__ ).fatal
#define qxpack_ic_info        QxPack::IcLogging().info
#define qxpack_ic_debug       QxPack::IcLogging( __FILE__, __LINE__ ).info
#define qxpack_ic_flush_info  QxPack::IcLogging().flushInfo
#define qxpack_ic_warning     QxPack::IcLogging( __FILE__, __LINE__ ).info
#define qxpack_ic_critical    QxPack::IcLogging( __FILE__, __LINE__ ).info

#endif

#endif
