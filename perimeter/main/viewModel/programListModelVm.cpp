#include "programListModelVm.h"
#include <perimeter/main/model/programModel.h>
#include <perimeter/main/services/translate_svc.h>

namespace Perimeter{

enum ProgramRoles
{
    program_id=Qt::UserRole + 1,
    type,
    name,
    category,
};

ProgramListModelVm::ProgramListModelVm(const QVariantList & list)
{
    m_category=list[0].toInt();
    qx_query query("select program_id,type,name,category from program where category=:category");
    query.bind(":category",m_category);
    Program_List program_List;
    QSqlError daoError = qx::dao::execute_query(query, program_List);
    beginResetModel();
    m_list=program_List;
    endResetModel();
}

void ProgramListModelVm::refreshData()
{
    qx_query query("select program_id,type,name,category,data from program where category=:category");
    query.bind(":category",m_category);
    Program_List program_List;
    QSqlError daoError = qx::dao::execute_query(query, program_List);
    beginResetModel();
    m_list=program_List;
    endResetModel();
}

//void ProgramListModelVm::hello()
//{
//    qDebug()<<m_str;
//}

int ProgramListModelVm::rowCount(const QModelIndex &) const
{
    return m_list.count();
}

QVariant ProgramListModelVm::data(const QModelIndex &index, int role) const
{
    int program_type;
    switch (role) {
    case program_id:return int(m_list[index.row()]->m_id);
    case type:return program_type=int(m_list[index.row()]->m_type);
    case name:
    {
        auto name = m_list[index.row()]->m_name;
        return TranslateController::getTranlatedName(name);
//        return name;
    }
    case category:return m_list[index.row()]->m_category;
//    case strategy:
//    {3
//        QString dataStr=m_list[index.row()]->m_data;
//        if(program_type==int(Type::ThreshHold))
//        {
//            auto dataModel=Utility::QStringToEntity<ProgramDataTraits<Type::ThreshHold>::data>(dataStr);
//            return QVariant(dataModel.strategies)
//            for(auto & i:dataModel.strategies)
//            {
//               QList<int> list;
//               list.append(int(i));
//               return list;
//            }
//        }
    default:return QVariant();
    }
}

QHash<int, QByteArray> ProgramListModelVm::roleNames() const
{
    QHash<int, QByteArray> roles;
       roles[program_id] = "program_id";
       roles[type]="type";
       roles[name] = "name";
       roles[category] = "category";
       return roles;
}

}
