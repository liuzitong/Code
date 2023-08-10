#include <QCoreApplication>
#include <qDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include <QThread>
#include <QMetaObject>

#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/afw/qxpack_ic_simpleactcaller.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjdeletor_priv.hxx"
#include "qxpack/indcom/sys/qxpack_ic_appdcl_priv.hxx"

// ////////////////////////////////////////////////////////////////////////////
// simple vm private object
// ////////////////////////////////////////////////////////////////////////////
class  SimpleVmPriv : public QObject {
    Q_OBJECT
private:
    QxPack::IcSimpleActCaller  m_act_caller;
public :
    // ctor
    SimpleVmPriv ( ) {
        qInfo() << "[ SimpleVmPriv ] running in QThread:" << QThread::currentThread();
        m_act_caller.build( this );
    }

    // dtor
    virtual ~SimpleVmPriv( ) Q_DECL_OVERRIDE { }

    // directly get the action caller pointer, user can call method directly
    inline QxPack::IcSimpleActCaller* actCaller_di( )
    { return & m_act_caller; }

    // action handler
    Q_INVOKABLE bool onSw_TestSw2 ( const QJsonObject &, const QVariantList &, int c_st, int t_st )
    {
        qInfo() << "SimpleVmPriv::onSw_ActionID_TestSw2() :" << c_st << " to " << t_st;
        return true;
    }
};

// ////////////////////////////////////////////////////////////////////////////
// simple vm object
// ////////////////////////////////////////////////////////////////////////////
class  SimpleVm : public QObject {
    Q_OBJECT
public:
    // ctor
    SimpleVm ( QThread *t ) {
        m_thread = t;
        m_obj = qobject_cast<SimpleVmPriv*>( QxPack::IcRmtObjCreator::createObjInThread (
            t, [](void*)->QObject* {
                return new SimpleVmPriv( );
            }, this
        ));
        QObject::connect(
            m_obj->actCaller_di(), SIGNAL(stateChanged(const QString&,int,int)),
            this, SIGNAL(stateChanged(const QString&,int,int))
        );
    }

    // dtor
    virtual ~SimpleVm ( ) Q_DECL_OVERRIDE
    {
        m_obj->actCaller_di()->blockBarrier(true); // make sure all done
        QObject::disconnect( m_obj, Q_NULLPTR, Q_NULLPTR, Q_NULLPTR );
        QxPack::IcRmtObjDeletor::deleteObjInThread(
            m_thread, []( void*, QObject* obj ) {
                obj->deleteLater();
            }, Q_NULLPTR, m_obj, true
        );
    }

    // method test post switch
    Q_INVOKABLE bool  testPostSw2( )
    {
        return m_obj->actCaller_di()->post( QStringLiteral("TestSw2"), QJsonObject());
    }

    // method test send switch
    Q_INVOKABLE bool  testSendSw2( )
    {
        return m_obj->actCaller_di()->send( QStringLiteral("TestSw2"), QJsonObject());
    }

    // test signal
    Q_SIGNAL void  stateChanged ( const QString &act, int curr_state, int target_state );

private:
    QThread      *m_thread;
    SimpleVmPriv *m_obj;
};


// ////////////////////////////////////////////////////////////////////////////
//      test object
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) {  }
Q_SLOT  void cleanupTestCase( );
Q_SLOT  void testCrInThreadAndPost( );
Q_SLOT  void testCrInMainAndSend( );
public :
    Test ( ) { }
    ~Test( ) { }
};

// ============================================================================
// final cleanup
// ============================================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" << QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// do the test routine
// ============================================================================
void  Test :: testCrInThreadAndPost( )
{
    QThread buss_thread;
    buss_thread.start();

    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
#ifdef TEST_USE_MEM_TRACE
    QxPack::IcMemCntr::enableMemTrace(true);
#endif

    {
        SimpleVm  simple_vm( &buss_thread );

        // this is used to quit
        QObject::connect(
            &simple_vm, & SimpleVm::stateChanged,
            []( const QString &act ,int c_st, int t_st ) {
                qInfo("Action(%s) changed: %d to %d", act.toUtf8().constData(), c_st, t_st );
                if ( act == QStringLiteral("TestSw2") ) {
                    QCoreApplication::instance()->quit();
                }
            }
        );

        // shed. a call to do test switch
        QMetaObject::invokeMethod (
            &simple_vm, "testPostSw2", Qt::QueuedConnection
        );
        QCoreApplication::instance()->exec();
    }

    buss_thread.quit();
    buss_thread.wait();
    QxPack::IcAppDclPriv::barrier();

#ifdef TEST_USE_MEM_TRACE
    QxPack::IcMemCntr::saveTraceInfoToFile( "z:\\t.txt");
#endif
    QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}


// ============================================================================
// do the test routine
// ============================================================================
void  Test :: testCrInMainAndSend( )
{
    int mem_cntr = QxPack::IcMemCntr::currNewCntr();
    {
        SimpleVm  simple_vm( Q_NULLPTR );

        // this is used to quit
        QObject::connect(
            &simple_vm, & SimpleVm::stateChanged,
            []( const QString &act ,int c_st, int t_st ) {
                qInfo("Action(%s) changed: %d to %d", act.toUtf8().constData(), c_st, t_st );
                if ( act == QStringLiteral("TestSw2") ) {
                    QCoreApplication::instance()->quit();
                }
            }
        );

        // shed. a call to do test switch
        QMetaObject::invokeMethod (
            &simple_vm, "testSendSw2", Qt::QueuedConnection
        );

        // main eventloop
        QCoreApplication::instance()->exec();
    }
    QxPack::IcAppDclPriv::barrier();

    QVERIFY2( mem_cntr == QxPack::IcMemCntr::currNewCntr(), "current memory counter is not equal" );
}





QTEST_MAIN( Test )
#include "main.moc"
