#include "table_model.h"
#include <QAbstractButton>
#include <QDebug>
#include <QEvent>
#include <QStyleOptionHeader>
#include <qstylepainter.h>


int TableModel::rowCount(const QModelIndex &parent) const
{
    return m_row;
}


int TableModel::columnCount(const QModelIndex &parent) const
{
    return m_column;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
           return QVariant();
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
       if(m_modelData==nullptr)
       {
           return "";
       }
       int data=m_modelData[index.row()*m_column+index.column()];

       if(data==-1) return "";
       return data;
    }
    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(m_modelData==nullptr)
    {
        return false;
    }
    if (index.isValid() && role == Qt::EditRole) {
        bool ok;
        int temp=value.toInt(&ok);
        if(!ok) temp=-1;
        m_modelData[index.row()*m_column+index.column()]=temp;
        emit dataChanged(index, index, QVector<int>() << role);
        if(m_func!=nullptr)
        { QString s=m_func(index.row(),index.column());}
        return true;
    }
    return false;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if(role == Qt::DisplayRole)
   {
       if( orientation == Qt::Horizontal)
       {
           if(section>m_hozHeader.count()-1)
           {
               return section;
           }
           return m_hozHeader[section];
       }
       else if(orientation==Qt::Vertical)
       {
           if(section>m_vertHeader.count()-1)
           {
               return section;
           }
           return m_vertHeader[section];
       }
   }

   return QAbstractTableModel::headerData(section,orientation,role);
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable|Qt::ItemIsEditable;
}



