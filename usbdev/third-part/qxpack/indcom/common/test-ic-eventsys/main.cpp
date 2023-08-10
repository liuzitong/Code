#include <QCoreApplication>
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include <QDebug>
#include <cstdlib>
#include <QObject>
#include <QtTest>
#include <string>
#include <cmath>
#include <QString>


// /////////////////////////////////////////////////
//
// test object
//
// /////////////////////////////////////////////////
class  Test : public QObject {
    Q_OBJECT
private:
Q_SLOT  void initTestCase( ) {  }
Q_SLOT  void cleanupTestCase ( );


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



QTEST_MAIN( Test )
#include "main.moc"
