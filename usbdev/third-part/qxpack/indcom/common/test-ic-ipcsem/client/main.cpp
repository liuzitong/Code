#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QtTest>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include "qxpack/indcom/common/qxpack_ic_ipcsem.hxx"

// ///////////////////////////////////////////////////////////
// main entry
// ///////////////////////////////////////////////////////////
int main( )
{
    bool ret = false;
    printf("child process started.\n");
    {
        QxPack::IcIpcSem  sem;
        sem.init( "TestIcIpcSem", 0 );
        printf("child create sem: %s\n", sem.isWorked() ? "OK" : "Err");
        ret = sem.wait( -1 ); // wait for inifinite
        printf("child wait for sem:%s\n", ret ? "OK" : "Err");
        fflush(stdout);
    }

    return ( ret ? 0 : -1 );
}

