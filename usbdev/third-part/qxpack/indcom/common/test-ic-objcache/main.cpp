#include <QCoreApplication>
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include <cmath>
#include <QString>
#include <QThread>

#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_objcache.hxx"


// ////////////////////////////////////////////////////////////////////////////
// NOTE: create this object in stack!!!
// ////////////////////////////////////////////////////////////////////////////
struct ObjPtrOprIF : public QxPack::IcObjCache::OprIF {
    virtual ~ObjPtrOprIF() override { }
    virtual void   deleteSelf()  override {  } //
    virtual int    verID()       override { return 0; }
    virtual void*  createObj ()  override { return qxpack_ic_alloc(64); }
    virtual void   deinitObj ( void* ) override { }
    virtual void   initObj   ( void* ) override { }
    virtual void   deleteObj ( void* ptr ) override { qxpack_ic_free( ptr ); }
};


// ////////////////////////////////////////////////////////////////////////////
// test object
// ////////////////////////////////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void  initTestCase( ) {  }
Q_SLOT  void  cleanupTestCase ( );
Q_SLOT  void  test_objcache();

public :
    Test ( ) { }
    ~Test( ) { }
};

// =================================================
// final cleanup
// =================================================
void  Test :: cleanupTestCase()
{
    qInfo() << "current MemCntr:" <<
            QxPack::IcMemCntr::currNewCntr();
}

// ============================================================================
// test the object cache
// ============================================================================
void  Test :: test_objcache()
{
    {
        ObjPtrOprIF  opr_if;
        QxPack::IcObjCache obj_cache( 16, 500, &opr_if );
        void *p1 = obj_cache.alloc();
        void *p2 = obj_cache.alloc();
        obj_cache.recycle( p1 );
        obj_cache.recycle( p2 );
        QThread::msleep(2000);
    }
    qInfo("current mem cntr:%d", QxPack::IcMemCntr::currNewCntr());
    QVERIFY2( QxPack::IcMemCntr::currNewCntr() == 0,"mem leak!" );
}


QTEST_MAIN( Test )
#include "main.moc"
