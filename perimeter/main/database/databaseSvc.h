#ifndef DATABASEVM_H
#define DATABASEVM_H
#include "perimeter/base/common/perimeter_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"
#include <QDate>
#include <QDateTime>
//#include <perimeter/main/model/patientListModel.h>
#include <perimeter/main/viewModel/patientVm.h>
#include <perimeter/main/viewModel/paramsVm.h>
#include "program.h"


namespace Perimeter {
class databaseSvc:public QObject
{
    Q_OBJECT
public:
    databaseSvc( );
    ~databaseSvc();
    Q_INVOKABLE void initDataBase();
    Q_INVOKABLE void createTable();
};


}
#endif // DATABASEVM_H
