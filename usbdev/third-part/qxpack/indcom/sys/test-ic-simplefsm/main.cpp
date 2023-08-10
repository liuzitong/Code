#include <QCoreApplication>
#include <QDebug>
#include <QtTest>
#include <QByteArray>
#include <QTimer>

#include "qxpack/indcom/sys/qxpack_ic_simplefsm.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_apptot_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"

// ////////////////////////////////////////////////////////////////////////////
//
//  traffic handler
//
// ////////////////////////////////////////////////////////////////////////////
class TraffiHandler : public QObject {
    Q_OBJECT
private:
    QxPack::IcSimpleFsm  m_fsm;
public :
    // ========================================================================
    // ctor
    // ========================================================================
    explicit TraffiHandler( )
    {
        m_fsm.init( this, "TraffStart", "TraffFinish" );
        m_fsm.postStart( ); // post a event to go to start State.
    }

    // ////////////////////////////////////////////////////////////////////////
    // FSM network graphic
    // ////////////////////////////////////////////////////////////////////////
    Q_SLOT bool onFsm_Red_Timeout_Yellow   ( const QVariantList & ) { return true; }
    Q_SLOT bool onFsm_Yellow_Timeout_Green ( const QVariantList & ) { return true; }
    Q_SLOT bool onFsm_Green_Timeout_TraffFinish ( const QVariantList & ) { return true; }

    // =======================================================================
    // the state 'TraffStart', while enter this state will trigger this
    // =======================================================================
    Q_SLOT void onFsm_TraffStart_Enter( void )
    {
        qInfo() << "The Traffic lamp start...";
        m_fsm.postGo("Red"); // postGo() is goto 'Red' state, will not call Red_XXXX_Go()
    }

    // ========================================================================
    // the state 'Red', while enter this state
    // ========================================================================
    Q_SLOT void onFsm_Red_Enter( void )
    {
        qInfo() << "Red on, wait for 2s to go Yellow";
        QTimer *tmr = new QTimer;
        QObject::connect(
            tmr, & QTimer::timeout,
            [this,tmr]() { this->m_fsm.postEvent( "Timeout" ); tmr->deleteLater(); }
        );
        tmr->setSingleShot(true);
        tmr->start(2000);
    }

    // ========================================================================
    // 'Yellow' state
    // ========================================================================
    Q_SLOT void onFsm_Yellow_Enter( void )
    {
        qInfo() << "Yellow on, wait for 1s to go Green";
        QTimer *tmr = new QTimer;
        QObject::connect(
            tmr, & QTimer::timeout,
            [this,tmr]() { this->m_fsm.postEvent( "Timeout" ); tmr->deleteLater(); }
        );
        tmr->setSingleShot(true);
        tmr->start(1000);
    }

    // ========================================================================
    // 'Green' state
    // ========================================================================
    Q_SLOT void onFsm_Green_Enter( void )
    {
        qInfo() << "Green on, wait for 2s to go Finish";
        QTimer *tmr = new QTimer;
        QObject::connect(
            tmr, & QTimer::timeout,
            [this,tmr]() { this->m_fsm.postEvent( "Timeout" ); tmr->deleteLater(); }
        );
        tmr->setSingleShot(true);
        tmr->start(2000);
    }

    // ========================================================================
    // 'TraffFinish' state
    // ========================================================================
    Q_SLOT void onFsm_TraffFinish_Enter( void )
    {
       qInfo() << "Finished";  QCoreApplication::quit();
    }
};


// ////////////////////////////////////////////////////////////////////////////
//
// test object
//
// ////////////////////////////////////////////////////////////////////////////
class  TestSimpleFsm : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) {  }
Q_SLOT  void cleanupTestCase  ( );
Q_SLOT  void testTrafficSample( );
public :
    TestSimpleFsm ( );
};

// ============================================================================
// ctor
// ============================================================================
    TestSimpleFsm :: TestSimpleFsm ( ) {  }

// ============================================================================
// final cleanup
// ============================================================================
void  TestSimpleFsm :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

void   TestSimpleFsm :: testTrafficSample()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    //QxPack::IcMemCntr::enableMemTrace( true );

    {
        TraffiHandler handler;
        QxPack::IcAppTotPriv::setTot( 60000 );
        QCoreApplication::exec();
    }
    QxPack::IcAppDclPriv::barrier();

    //QxPack::IcMemCntr::saveTraceInfoToFile("z:/t.txt");;
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "exist memory leak!" );

}



QTEST_MAIN( TestSimpleFsm )
#include "main.moc"
