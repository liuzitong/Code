#include <QCoreApplication>

#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include "qxpack/indcom/common/qxpack_ic_logging.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_tmrqueue.hxx"

#include <QString>
#include <QDebug>
#include <cstdlib>
#include <QSemaphore>
#include <QObject>
#include <QtTest>
#include <QElapsedTimer>

static QElapsedTimer g_elap_tmr;

// ////////////////////////////////////////////////////////////////////////////
// test object
// ////////////////////////////////////////////////////////////////////////////
class  TestTmrQueue : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) { }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void testSingleShot( );
Q_SLOT  void testRepeat( );
    void testDtorCancel( );
//Q_SLOT  void testAutoClear ( );
public :
    TestTmrQueue ( );
};

// ============================================================================
// CTOR
// ============================================================================
    TestTmrQueue :: TestTmrQueue ( ) {  }

// ============================================================================
// final cleanup
// ============================================================================
void  TestTmrQueue :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test the timer
// ============================================================================
void   TestTmrQueue :: testSingleShot()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    QxPack::IcMemCntr::enableMemTrace( false );
    {
        struct ctxt_t { QSemaphore m_sem; QElapsedTimer m_elap; };
        ctxt_t context;

        QxPack::IcTmrQueue  tmr_q;
        QxPack::IcVariant   var_ctxt( &context );
        context.m_elap.start();

        tmr_q.singleShot(
            64, []( QxPack::qxic_uid_t  id,
            const QxPack::IcVariant &ctxt, const QxPack::IcVariant &, const QxPack::IcVariant&, const QxPack::IcVariant&, bool is_valid ) {
                ctxt_t *cp =   reinterpret_cast<ctxt_t*>(ctxt.toVoidStar());
                qInfo("single shot call: %lld ( %s ) called [ %lld ]", id, ( is_valid ? "valid" : "invalid"), cp->m_elap.elapsed() );
                cp->m_sem.release();
            }, var_ctxt, QxPack::IcVariant()
        );

        tmr_q.singleShot(
            1000,
            []( QxPack::qxic_uid_t id, const QxPack::IcVariant &ctxt, const QxPack::IcVariant&, const QxPack::IcVariant&, const QxPack::IcVariant&, bool is_valid ) {
                ctxt_t *cp = reinterpret_cast<ctxt_t*>(ctxt.toVoidStar());
                qInfo("single shot call: %lld ( %s ) called [ %lld ]", id, ( is_valid ? "valid" : "invalid"), cp->m_elap.elapsed() );
                cp->m_sem.release();
            }, var_ctxt, QxPack::IcVariant()
        );

        context.m_sem.acquire(2); // wait..
    }
    QxPack::IcMemCntr::saveTraceInfoToFile( "z:/t.txt");
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "memory leak!" );
}

// ============================================================================
// do 10s repeat call
// ============================================================================
static void  gRepeatSS(
    QxPack::qxic_uid_t id,
    const QxPack::IcVariant &ctxt, const QxPack::IcVariant &,
    const QxPack::IcVariant &, const QxPack::IcVariant &,
    bool is_valid
) {
    if ( is_valid ) {
        QxPack::IcTmrQueue *tmr_q = reinterpret_cast<QxPack::IcTmrQueue*>( ctxt.toVoidStar() );
        qInfo("repeat call at %lld ", g_elap_tmr.elapsed());
        tmr_q->singleShot( 100, &gRepeatSS, ctxt, QxPack::IcVariant() );
    }
}

void   TestTmrQueue :: testRepeat()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    QxPack::IcMemCntr::enableMemTrace( false );
    {
        QxPack::IcTmrQueue  tmr_q;
        QxPack::IcVariant   var( &tmr_q );
       QSemaphore  sem;
        g_elap_tmr.restart();
        tmr_q.singleShot( 100, & gRepeatSS, var, QxPack::IcVariant() );

        sem.tryAcquire(3,20000); // wait..
    }
    QxPack::IcMemCntr::saveTraceInfoToFile( "z:/t.txt");
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "memory leak!" );
}

// ============================================================================
// test the timer
// ============================================================================
void   TestTmrQueue :: testDtorCancel()
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    QxPack::IcMemCntr::enableMemTrace( false );
    {
        struct ctxt_t { QSemaphore m_sem; QElapsedTimer m_elap; };
        ctxt_t context;
        context.m_elap.start();

        QxPack::IcTmrQueue  tmr_q;
        QxPack::IcVariant   var( &context );

        tmr_q.singleShot (
            3000,
            []( QxPack::qxic_uid_t  id, const QxPack::IcVariant &ctxt, const QxPack::IcVariant &, const QxPack::IcVariant&, const QxPack::IcVariant&, bool is_valid ) {
                if ( is_valid ) {
                    ctxt_t *cp = reinterpret_cast<ctxt_t*>(ctxt.toVoidStar());
                    qInfo("single shot call: %lld ( %s ) called [ %lld ]", id, ( is_valid ? "valid" : "invalid"), cp->m_elap.elapsed() );
                    cp->m_sem.release();
                } else {
                    qInfo("single shot call: %lld ( %s ) called", id, ( is_valid ? "valid" : "invalid"));
                }
            }, var, QxPack::IcVariant()
        );

        context.m_sem.tryAcquire(1,2000);
    }
    QxPack::IcMemCntr::saveTraceInfoToFile( "z:/t.txt");
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == mem_cntr, "memory leak!" );
}


QTEST_MAIN( TestTmrQueue )
#include "main.moc"
