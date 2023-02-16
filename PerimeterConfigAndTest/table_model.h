#ifndef COLOR_SLOT_MODEL_H
#define COLOR_SLOT_MODEL_H
#include <QAbstractTableModel>
//#include <spdlog/spdlog.h>
#include <QJsonDocument>
//#include <functional>
#include <QSharedPointer>



//颜色  光斑
class TableModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(int* data,int row,int column,QList<QString> hozHeader,QList<QString> vertHeader,std::function<QString(int,int)> func=nullptr,QObject* parent=nullptr)
        :QAbstractTableModel(parent),m_modelData(data),m_row(row),m_column(column),m_hozHeader(hozHeader),m_vertHeader(vertHeader),m_func(func){};
    TableModel()=default;
    ~TableModel()=default;

public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    void printData();

    int* m_modelData=nullptr;
    int m_row=0,m_column=0;
    QList<QString> m_hozHeader;
    QList<QString> m_vertHeader;
public:
    std::function<QString(int,int)> m_func=nullptr;
};

#endif // COLOR_SLOT_MODEL_H
