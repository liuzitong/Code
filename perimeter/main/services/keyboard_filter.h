#ifndef KEYBOARD_FILTER_H
#define KEYBOARD_FILTER_H

#include <QObject>
namespace Perimeter{
class KeyBoardFilter : public QObject
{
    Q_OBJECT
public:
    explicit KeyBoardFilter(QObject *parent = nullptr);
    static bool answered;
    static bool freshed;
    static bool needRefresh;

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
private:
    bool keyAPressed=false;
    bool keySPressed=false;
    bool keyDPressed=false;

signals:

public slots:
};
}
#endif // KEYBOARD_FILTER_H
