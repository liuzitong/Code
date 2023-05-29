#include "keyboard_filter.h"
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
namespace Perimeter{

bool KeyBoardFilter::answered=false;
bool KeyBoardFilter::freshed=false;
bool KeyBoardFilter::needRefresh=true;

KeyBoardFilter::KeyBoardFilter(QObject *parent) : QObject(parent)
{

}

bool KeyBoardFilter::eventFilter(QObject *obj, QEvent *event)
{

    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if(keyEvent->type()==QKeyEvent::KeyPress)
    {
        if (keyEvent->key() == Qt::Key_A)
        {
            keyAPressed=true;
        }
        else if(keyEvent->key() == Qt::Key_S)
        {
            keySPressed=true;
        }
        else if(keyEvent->key() == Qt::Key_D)
        {
            keyDPressed=true;
        }
    }
    else if(keyEvent->type()==QKeyEvent::KeyRelease)
    {
        if (keyEvent->key() == Qt::Key_A&&keyAPressed)
        {
            answered=true;
            freshed=true;
            keyAPressed=false;
        }
        else if (keyEvent->key() == Qt::Key_S&&keySPressed)
        {
            answered=false;
            freshed=true;
            keySPressed=false;
        }
        else if (keyEvent->key() == Qt::Key_D&&keyDPressed)
        {
            keyDPressed=false;
            needRefresh=!needRefresh;
        }
    }
    //同理，必须让事件循环继续，否则在这里就中断了
    return QObject::eventFilter(obj, event);
}
}
