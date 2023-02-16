#include "generic_table.h"

#include <QEvent>
#include <qabstractbutton.h>
#include <qapplication.h>
#include <qstylepainter.h>
#include <QHeaderView>
#include <QPushButton>

GenericTable::GenericTable(QWidget *parent):QTableView(parent)
{
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void GenericTable::setCornerName(QString cornerName)
{
    QAbstractButton* btn =findChild<QAbstractButton*>();
    if (btn)
    {
        btn->setText(cornerName);
        btn->installEventFilter(this);
//        QStyleOptionHeader opt;
//        opt.text = btn->text();
//        btn->setStyleSheet(R"(QAbstractButton::section{text-align:center})");

//        QSize s = (btn->style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), btn).
//                   expandedTo(QApplication::globalStrut()));
//        if (s.isValid())
        verticalHeader()->setMinimumWidth(60);
    }

}


bool GenericTable::eventFilter(QObject* o, QEvent* e)
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
