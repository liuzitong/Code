#ifndef QXPACK_IC_APPTOT_PRIV_HXX
#define QXPACK_IC_APPTOT_PRIV_HXX

#include <QObject>
#include <QTimer>
#include <QCoreApplication>
#include <QDebug>

namespace QxPack {

class  IcAppTotPriv {

public :  typedef void (* Callback )( void *ctxt, IcAppTotPriv &, bool is_limit );
private:  QTimer m_limit_tmr, m_cb_tmr; Callback m_cb_func; void *m_cb_obj;
public :
    // =============================================================
    // construct a AppTot object with limit ms and callback ms
    // =============================================================
    IcAppTotPriv( int limit_ms, int cb_ms, Callback cb, void* cb_ctxt )
    {
        m_limit_tmr.setInterval (( limit_ms <= 0 ? 16 : limit_ms ));
        if ( cb_ms > 0 ) { m_cb_tmr.setInterval ( cb_ms ); }
        m_cb_func = cb; m_cb_obj = cb_ctxt;

        QObject::connect(
            & m_limit_tmr, & QTimer::timeout,
            [this]() {
                this->m_cb_tmr.stop();
                this->m_limit_tmr.stop();
                if ( this->m_cb_func != Q_NULLPTR ) {
                    (* this->m_cb_func )( this->m_cb_obj, *this, true );
                }
            }
        );
        QObject::connect(
            & m_cb_tmr, & QTimer::timeout,
            [this]() {
                if ( this->m_cb_func != Q_NULLPTR ) {
                    (* this->m_cb_func )( this->m_cb_obj, *this, false );
                }
            }
        );

        m_limit_tmr.setSingleShot( true ); m_limit_tmr.start();
        if ( cb_ms > 0 ) {
            m_cb_tmr.setSingleShot( false );   m_cb_tmr.start();
        }
    }

    virtual ~IcAppTotPriv( )
    {
        m_cb_tmr.stop();
        m_limit_tmr.stop();
    }

    inline void  stopTimer( ) { m_cb_tmr.stop(); m_limit_tmr.stop(); }
    Q_DISABLE_COPY( IcAppTotPriv )



public :
    // =============================================================
    //! set a limit timer on QCoreApplication eventloop
    // =============================================================
    inline static void setTot( int ms )
    {
        QTimer *tmr = new QTimer;
        QObject::connect( 
            tmr, & QTimer::timeout, 
            [](){
                qInfo() << "IcAppTotPriv::timeout!";
                QCoreApplication::quit();
            }
        );
        QObject::connect(
            QCoreApplication::instance(), SIGNAL(aboutToQuit()),
            tmr, SLOT(stop())
        );
        QObject::connect(
            QCoreApplication::instance(), SIGNAL(aboutToQuit()),
            tmr, SLOT(deleteLater())
        );
        QObject::connect(
            tmr, & QTimer::destroyed,
            [](){  }
        );
        tmr->start( ms );
    }
};


}
#endif
