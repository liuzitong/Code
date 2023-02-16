#ifndef NORMAL_TABLE_H
#define NORMAL_TABLE_H

#include "table_model.h"

#include <QObject>
#include <QTableView>

class GenericTable: public QTableView
{
    Q_OBJECT
public:
    GenericTable(QWidget *parent);
    void setCornerName(QString cornerName);
        ~GenericTable()=default;
    bool eventFilter(QObject *o, QEvent *e);

};

#endif // NORMAL_TABLE_H
