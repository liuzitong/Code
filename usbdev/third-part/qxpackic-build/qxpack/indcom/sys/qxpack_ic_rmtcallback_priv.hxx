#ifndef QXPACK_IC_RMTCALLBACK_PRIV_HXX
#define QXPACK_IC_RMTCALLBACK_PRIV_HXX

#include "../common/qxpack_ic_def.h"
#include <QThread>
#include <QSemaphore>

namespace QxPack {


// ////////////////////////////////////////////////////////////////////////////
//
//                      Remote call back
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcRmtCallback {
public:
    typedef void (* CallbackFunc )( void* ctxt );

    class Trigger {
    private:
        void *m_obj;
        friend class IcRmtCallback;
    public :
        explicit Trigger( );
        Trigger( const Trigger & );
        Trigger & operator = ( const Trigger & );
#ifdef QXPACK_IC_CXX11
        Trigger ( Trigger && );
        Trigger & operator = ( Trigger && );
#endif
        ~Trigger( );
        void  reqTrigger( );
    };

    static void     postDelayedCallbackInThread( QThread *t, CallbackFunc f, void *ctxt, int ms );
    static void     barrierInThread( QThread *t ) { postDelayedCallbackInThread( t, [](void*){}, Q_NULLPTR, 0 ); }
    static Trigger  initTrgCallbackInThread( QThread *t, CallbackFunc f, void *ctxt, int trg_tm );

protected:
    IcRmtCallback( );
    virtual ~IcRmtCallback( );
private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcRmtCallback )
};

}

#endif
