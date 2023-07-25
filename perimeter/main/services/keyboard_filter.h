#ifndef KEYBOARD_FILTER_H
#define KEYBOARD_FILTER_H

#include <QObject>
namespace Perimeter{
class KeyBoardFilter : public QObject
{
    Q_OBJECT
public:
    explicit KeyBoardFilter(QObject *parent = nullptr);
    static QSharedPointer<KeyBoardFilter> getSingleton();
    static bool answered;
    static bool freshed;
    static bool needRefresh;
    static bool showDeviceStatusData;

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
private:
    bool keyAPressed=false;
    bool keySPressed=false;
    bool keyDPressed=false;
    bool keyAltPressed=false;
    bool keyRPressed=false;
    static QSharedPointer<KeyBoardFilter> m_singleton;

signals:
    void showDeviceStatusChanged();


public slots:
};
}
#endif // KEYBOARD_FILTER_H
