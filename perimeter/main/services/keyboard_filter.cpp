#include "keyboard_filter.h"
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <qmutex.h>
namespace Perimeter{

bool KeyBoardFilter::answered=false;
bool KeyBoardFilter::freshed=false;
bool KeyBoardFilter::needRefresh=true;
bool KeyBoardFilter::showDeviceStatusData=false;
QSharedPointer<KeyBoardFilter> KeyBoardFilter::m_singleton=nullptr;

KeyBoardFilter::KeyBoardFilter(QObject *parent) : QObject(parent)
{

}

QSharedPointer<KeyBoardFilter> KeyBoardFilter::getSingleton()
{
    static QMutex mutex;
    mutex.lock();
    if(m_singleton==nullptr)
    {
        m_singleton.reset(new KeyBoardFilter());
    }
    mutex.unlock();
    return m_singleton;
}

bool KeyBoardFilter::eventFilter(QObject *obj, QEvent *event)
{

    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if(keyEvent->type()==QKeyEvent::KeyPress)
    {
        switch (keyEvent->key())
        {
        case  Qt::Key_A: keyAPressed=true;break;
        case  Qt::Key_S: keySPressed=true;break;
        case  Qt::Key_D: keyDPressed=true;break;
        case  Qt::Key_Alt: keyAltPressed=true;break;
        case  Qt::Key_R: keyRPressed=true;break;
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
        else if(keyEvent->key() == Qt::Key_Alt)
        {
            keyAltPressed=false;
        }
        else if(keyEvent->key() == Qt::Key_R)
        {
            if(keyAltPressed&&keyRPressed)
            {
                showDeviceStatusData=!showDeviceStatusData;
                qDebug()<<showDeviceStatusData;
                emit showDeviceStatusChanged();
            }
            keyRPressed=false;
        }
    }
    //同理，必须让事件循环继续，否则在这里就中断了
    return QObject::eventFilter(obj, event);
}
}
