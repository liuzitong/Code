#ifndef MOTORPOSTABLE_H
#define MOTORPOSTABLE_H

#include <QWidget>
#include <QTableView>
#include <QVariant>
#include "RbTableHeaderView.h"
#include <table_model.h>
#include <QSharedPointer>

class MotorPosTable : public QTableView
{
    Q_OBJECT
public:
    MotorPosTable(QWidget *parent);
    void setData(int* data);
    void initTable();
        ~MotorPosTable()=default;
    bool eventFilter(QObject *o, QEvent *e) override;
//signals:
//    void requestPaint();
public:
    TableModel* m_tableModel;
    static constexpr int columnCount=(180/6+1);
    static constexpr int rowCount=(180/6+1);
    RbTableHeaderView* hHead,*vHead;
    QAbstractItemModel* hModel,*vModel;

protected slots:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

};

#endif // MOTORPOSTABLE_H
