#include "precompiled.h"
#include "databaseSvc.h"
#include "checkResult.h"
#include "patient.h"
#include "program.h"
//#include <perimeter/main/model/patientListModel.h>
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include <QFile>
#include <QDebug.h>

namespace Perimeter {

databaseSvc::databaseSvc()
{
    initDataBase();
}

databaseSvc::~databaseSvc()
{
}

void databaseSvc::initDataBase()
{
    QFileInfo fi((R"(./db.sqlite)"));
    bool isDbThere=fi.exists();
    qx::QxSqlDatabase::getSingleton()->setDriverName("QSQLITE");
    qx::QxSqlDatabase::getSingleton()->setDatabaseName(R"(./db.sqlite)");
    qx::QxSqlDatabase::getSingleton()->setHostName("localhost");
    qx::QxSqlDatabase::getSingleton()->setUserName("root");
    qx::QxSqlDatabase::getSingleton()->setPassword("");
    qx::QxSqlDatabase::getSingleton()->setFormatSqlQueryBeforeLogging(true);
    qx::QxSqlDatabase::getSingleton()->setDisplayTimerDetails(true);
    qx::QxSqlDatabase::getSingleton()->setVerifyOffsetRelation(true);
    if(!isDbThere)
        createTable();
}


void databaseSvc::createTable(){
    QSqlError daoError = qx::dao::create_table<Patient>();
    daoError = qx::dao::create_table<CheckResult>();
    daoError = qx::dao::create_table<Program>();
}


}
