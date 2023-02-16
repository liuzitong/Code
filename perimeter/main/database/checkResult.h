#ifndef CHECK_RESULT_H
#define CHECK_RESULT_H
#include "precompiled.h"
#include <QxOrm_Impl.h>
#include <QDate>
#include <QJsonObject>
class Patient;
class Program;

class QX_DLL_EXPORT_API CheckResult{
public:
// -- typedef
    typedef std::shared_ptr<Patient> Patient_ptr;
    typedef std::shared_ptr<Program> Program_ptr;

    long m_id;
    int m_type;
    int m_OS_OD;                       //0左眼 OS,1右眼 OD
    QString m_params;
    QString m_data;
    QByteArray m_blob;
    QString m_diagnosis;
    QDateTime m_time;
    Patient_ptr m_patient;
    Program_ptr m_program;
    CheckResult() = default;
    CheckResult(int type,int OS_OD,QString params,QString data,QDateTime time,Patient_ptr patient,Program_ptr program):
    m_type(type),m_OS_OD(OS_OD),m_params(params),m_data(data),m_time(time),m_patient(patient),m_program(program){}
    CheckResult(long id,int type,int OS_OD,QString params,QString data,QDateTime time,Patient_ptr patient,Program_ptr program):
    m_id(id),m_type(type),m_OS_OD(OS_OD),m_params(params),m_data(data),m_time(time),m_patient(patient),m_program(program){}
    virtual ~CheckResult(){}
};

QX_REGISTER_PRIMARY_KEY(CheckResult, long)
QX_REGISTER_HPP_QX_API(CheckResult, qx::trait::no_base_class_defined, 0)

typedef std::shared_ptr<CheckResult> CheckResult_ptr;
typedef QList<CheckResult_ptr> CheckResult_List;

#endif // CHECK_RESULT_H


//class CheckResultVm{
//public:
//    Q_PROPERTY()
//}
