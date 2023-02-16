#ifndef PROGRAM_H
#define PROGRAM_H
#include "precompiled.h"
#include <QxOrm_Impl.h>
#include <QDate>
#include <QPoint>
#include <QVector>
class CheckResult;
class QX_DLL_EXPORT_API Program{
public:
// -- typedef
    typedef std::shared_ptr<CheckResult> CheckResult_ptr;
    typedef std::vector<CheckResult_ptr> CheckResult_List;

    long m_id;
    int m_type;
    QString m_name;
    QString m_params;
    QString m_data;
    QString m_report;
    int m_category;
    CheckResult_List m_checkResultX;
    Program()=default;
    Program(int type,QString name,QString params,QString data,QString report,int category=4):
        m_type(type),m_name(name),m_params(params),m_data(data),m_report(report),m_category(category){}
    Program(long id,int type,QString name,QString params,QString data,QString report,int category=4):
        m_id(id),m_type(type),m_name(name),m_params(params),m_data(data),m_report(report),m_category(category){}
    virtual ~Program(){}
};

QX_REGISTER_PRIMARY_KEY(Program, long)
QX_REGISTER_HPP_QX_API(Program, qx::trait::no_base_class_defined, 0)

typedef std::shared_ptr<Program> Program_ptr;
//typedef qx::QxCollection<long, Program_ptr> Program_List;
typedef QList<Program_ptr> Program_List;
#endif // PROGRAM_H
