#ifndef QXPACK_IC_UI_QML_API_HXX
#define QXPACK_IC_UI_QML_API_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <QObject>
#include <QString>
#include <QVariant>
#include <QQmlEngine>
#include <QJSEngine>
#include <QSharedPointer>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief this class support a set of APIs in UI(QML)
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcUiQmlApi : public QObject {
    Q_OBJECT
    Q_PROPERTY( QString   apiVersion READ apiVersion CONSTANT )
    Q_PROPERTY( QObject*  appCtrl    READ appCtrl    CONSTANT )
public:
    //! CTOR
    IcUiQmlApi ( QQmlEngine *, QJSEngine* );
    IcUiQmlApi ( );

    //! DTOR
    virtual ~IcUiQmlApi( ) Q_DECL_OVERRIDE;

    //! create the instance of IcUiQmlApi
    static QObject *  createInstance( QQmlEngine*, QJSEngine* );

    //! set the appctrl object ( nw: 2019/04/20 )
    //! @param app_ctrl [in] the application controller shared pointer
    //! @warning this function should be called before Qml scene start
    static void  setAppCtrl( const QSharedPointer<QObject> &app_ctrl );

    //! register qml in qmlengine ( nw: 2019/05/07 )
    static void  registerApi( );

    //! return current application controller
    QObject*  appCtrl() const;

    //! return the version of this API set
    QString  apiVersion() const;

    //! enter a new QEventLoop
    Q_INVOKABLE void  enterEventLoop( );

    //! leave a event loop
    Q_INVOKABLE void  leaveEventLoop( );

    //! post the queued method call
    Q_INVOKABLE  bool  postMetaCall(
        QObject* obj, const QString & method,
        const QVariant & = QVariant(), const QVariant & = QVariant(),
        const QVariant & = QVariant(), const QVariant & = QVariant()
    );

    // this signal is connected to QCoreApplication::aboutToQuit()
    Q_SIGNAL void  aboutToQuit();

private:
    void *m_obj;
    Q_DISABLE_COPY( IcUiQmlApi )
};


}

#endif
