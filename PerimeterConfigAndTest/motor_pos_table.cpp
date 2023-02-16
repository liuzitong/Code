#include "motor_pos_table.h"

#include <QPushButton>
#include <qabstractbutton.h>
#include <qapplication.h>
#include <qstylepainter.h>
#include <QDebug>
#include <QThread>

MotorPosTable::MotorPosTable(QWidget *parent):QTableView(parent)
{

    initTable();
    hHead = new RbTableHeaderView(Qt::Horizontal,1,columnCount);
    hModel = hHead->model();
    for(int i=0;i<columnCount;i++)
        hModel->setData(hModel->index(0,i),QString::number(-90+i*6),Qt::DisplayRole);


    vHead = new RbTableHeaderView(Qt::Vertical,rowCount*3+1,2);
    vModel = vHead->model();

    for(int i=0;i<rowCount;i++)
    {
        vHead->setSpan(i*3,0,3,1);
        vModel->setData(vModel->index(i*3,0),QString::number(90-i*6),Qt::DisplayRole);
        vModel->setData(vModel->index(i*3,1),QStringLiteral("X步"),Qt::DisplayRole);
        vModel->setData(vModel->index(i*3+1,1),QStringLiteral("Y步"),Qt::DisplayRole);
        vModel->setData(vModel->index(i*3+2,1),QStringLiteral("距离"),Qt::DisplayRole);
    }

    for(int i=0;i<columnCount;i++)
        hHead->setColumnWidth(i,58);

    for(int i=0;i<rowCount*3;i++)
        vHead->setRowHeight(i,20);

    vHead->setRowHeight(rowCount*3,0);
    vHead->setSpan(rowCount*3,0,0,2);

    setHorizontalHeader(hHead);
    setVerticalHeader(vHead);


    QAbstractButton *btn = findChild<QAbstractButton*>();
    if (btn)
    {
        btn->setText(QStringLiteral("Y/X"));
        btn->installEventFilter(this);/*注册事件 eventFilter*/
        QStyleOptionHeader opt;
        opt.text = btn->text();
        QSize s = (btn->style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), btn).expandedTo(QApplication::globalStrut()));
        if (s.isValid())
           verticalHeader()->setMinimumWidth(s.width());
    }


//    setStyleSheet(R"(QTableCornerButton::section{
//                  border: 1px solid white;
//                  text-align: center;
//                  text-indent: 30px;
//                  })");
//            connect(hHead,SIGNAL(sectionPressed(int,int)),this,SLOT(horizontalHeaderSection_Pressed(int,int)));
    //            connect(vHead,SIGNAL(sectionPressed(int,int)),this,SLOT(verticalHeaderSection_Pressed(int,int)));
}

void MotorPosTable::setData(int *data)
{
    m_tableModel->m_modelData=data;
}


void MotorPosTable::initTable()
{
//    auto motordata=new int[columnCount*(rowCount*3)]{0};

//    for(int i=0;i<columnCount*(rowCount*3);i++)
//    {
//        motordata[i]=0;
//    }
    m_tableModel=new TableModel();
    m_tableModel->m_column=columnCount;
    m_tableModel->m_row=rowCount*3;
//    m_tableModel->m_modelData=motordata;
    setModel(m_tableModel);
}


bool MotorPosTable::eventFilter(QObject* o, QEvent* e)
{
    if (e->type() == QEvent::Paint)
    {
        QAbstractButton* btn = qobject_cast<QAbstractButton*>(o);
        if (btn)
        {
            // paint by hand (borrowed from QTableCornerButton)

            QStyleOptionHeader opt;
            opt.init(btn);
            QStyle::State state = QStyle::State_None;
            if (btn->isEnabled())
                state |= QStyle::State_Enabled;
            if (btn->isActiveWindow())
                state |= QStyle::State_Active;
            if (btn->isDown())
                state |= QStyle::State_Sunken;
            opt.state = state;
            opt.rect = btn->rect();
            opt.text = btn->text(); // this line is the only difference to QTableCornerButton

            opt.position = QStyleOptionHeader::OnlyOneSection;
            QStylePainter painter(btn);
            painter.drawControl(QStyle::CE_Header, opt);
            return true; // eat event

        }
    }
    return false;
}

void MotorPosTable::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if(deselected.count()!=0)
    {
        for(auto index :deselected.indexes())
        {
            auto col=index.column();
            auto row=index.row();
            hHead->setCellBackgroundColor(hModel->index(0,col),0xefebe7);
            vHead->setCellBackgroundColor(vModel->index(int(row/3)*3,0),0xefebe7);
        }

        vHead->viewport()->update();
        hHead->viewport()->update();
//        emit requestPaint();


    }

    if(selected.count()!=0)
    {
        for(auto index :selected.indexes())
        {
            auto col=index.column();
            auto row=index.row();
            hHead->setCellBackgroundColor(hModel->index(0,col),0x96eaf5);
            vHead->setCellBackgroundColor(vModel->index(int(row/3)*3,0),0x96eaf5);
        }
        vHead->viewport()->update();
        hHead->viewport()->update();
    }
}


//void MotorPosTable::horizontalHeaderSection_Pressed(int beginSection, int endSection)
//{
//    clearSelection();
//    QAbstractItemView::SelectionMode oldSelectionMode = selectionMode();
//    setSelectionMode(QAbstractItemView::MultiSelection);
//    for (int i=beginSection;i<endSection+1;++i)
//        selectColumn(i);
//    setSelectionMode(oldSelectionMode);
//}

//void MotorPosTable::verticalHeaderSection_Pressed(int beginSection, int endSection)
//{
//    clearSelection();
//    QAbstractItemView::SelectionMode oldSelectionMode = selectionMode();
//    setSelectionMode(QAbstractItemView::MultiSelection);
//    for (int i=beginSection;i<endSection+1;++i)
//        selectRow(i);
//    setSelectionMode(oldSelectionMode);
//}


