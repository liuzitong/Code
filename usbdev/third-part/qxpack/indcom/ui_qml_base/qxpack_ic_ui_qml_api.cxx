#ifndef QXPACK_IC_UI_QML_API_CXX
#define QXPACK_IC_UI_QML_API_CXX

#include "qxpack_ic_ui_qml_api.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_dyncinit_priv.hxx"

#include <QMetaObject>
#include <QEventLoop>
#include <QStack>
#include <QtGlobal>
#include <QCoreApplication>
#include <QDebug>

namespace QxPack {
    void QXPACK_IC_HIDDEN IcUiQmlBase_staticInit();
    void QXPACK_IC_HIDDEN IcUiQmlControl_staticInit();
    void QXPACK_IC_HIDDEN IcUiQmlCharts_staticInit();
}


// ////////////////////////////////////////////////////////////////////////////
//
// register at QCoreApplication Event begin
//
// ////////////////////////////////////////////////////////////////////////////
static void gRegUiQmlApi( )
{ 
    static bool is_reg = false;
    if ( ! is_reg ) {
        is_reg = true;
        qmlRegisterSingletonType<QxPack::IcUiQmlApi>( "qxpack.indcom.ui_qml_base", 1, 0, "IcUiQmlApi", & QxPack::IcUiQmlApi::createInstance );
        qInfo() << "qxpack_ic_ui_qml_api registered."; // nw: 2019/05/07 added
    }

    QxPack::IcUiQmlBase_staticInit();
#ifndef QXPACK_IC_NO_UI_QML_CONTROL
    QxPack::IcUiQmlControl_staticInit();
#endif
#ifndef QXPACK_IC_NO_UI_QML_CHARTS
    QxPack::IcUiQmlCharts_staticInit();
#endif

}

// ============================================================================
// use below macro to do register. this will be run at
// the end of QCoreApplication constructor
// ============================================================================
Q_COREAPP_STARTUP_FUNCTION( gRegUiQmlApi )


namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// local functions
// ////////////////////////////////////////////////////////////////////////////
static QSharedPointer<QObject>  g_app_ctrl;

// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPtr( o )  T_ObjCast( IcUiQmlApiPriv*, o )
class QXPACK_IC_HIDDEN  IcUiQmlApiPriv {
private:
    QQmlEngine *m_qml_eng;
    QJSEngine  *m_js_eng;
    QStack<void*> m_evt_loop_stack;
public :
    IcUiQmlApiPriv ( QQmlEngine *, QJSEngine *);
    virtual ~IcUiQmlApiPriv( );
    inline QStack<void*> &  evtLoopStack() { return m_evt_loop_stack; }
};

// ============================================================================
// ctor
// ============================================================================
IcUiQmlApiPriv :: IcUiQmlApiPriv (QQmlEngine *qml, QJSEngine *js)
{
    m_qml_eng = qml; m_js_eng = js;
    if ( qml != Q_NULLPTR ) {
        qml->addImportPath( QStringLiteral("qrc:/") );
    }
}

// ============================================================================
// dtor
// ============================================================================
IcUiQmlApiPriv :: ~IcUiQmlApiPriv ( )
{

}



// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcUiQmlApi :: IcUiQmlApi ( QQmlEngine *q, QJSEngine *j )
{
    m_obj = qxpack_ic_new( IcUiQmlApiPriv, q, j );
    QObject::connect( QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SIGNAL(aboutToQuit()) );
}

// ============================================================================
// dtor
// ============================================================================
IcUiQmlApi :: ~IcUiQmlApi ( )
{
    if ( m_obj != Q_NULLPTR ) {
        qxpack_ic_delete( m_obj, IcUiQmlApiPriv );
    }
}

// ============================================================================
// [ static ] create an instance for Engine
// NOTE: this object will owned by QmlEngine
// ============================================================================
QObject *    IcUiQmlApi::createInstance( QQmlEngine *q, QJSEngine *j )
{
    return qobject_cast<QObject*>( new IcUiQmlApi( q, j ));
}

// ============================================================================
// [ static ] set the application controller
// ============================================================================
void        IcUiQmlApi :: setAppCtrl( const QSharedPointer<QObject> &app_ctrl )
{
    g_app_ctrl = app_ctrl;
}

// ============================================================================
// [ static ] register in qmlengine
// ============================================================================
void        IcUiQmlApi :: registerApi ( ) { gRegUiQmlApi(); } // nw: 2019/05/07 added

// ============================================================================
// return the app ctrl
// ============================================================================
QObject*    IcUiQmlApi :: appCtrl() const
{   return  g_app_ctrl.data(); }

// ============================================================================
// return an api version
// ============================================================================
QString     IcUiQmlApi :: apiVersion() const
{
    return QStringLiteral("1.1");
}

// ============================================================================
// enter a new eventloop
// ============================================================================
void  IcUiQmlApi  :: enterEventLoop ( )
{
    QEventLoop *evt = new QEventLoop();
    T_PrivPtr( m_obj )->evtLoopStack().push( evt );
    evt->exec();
    evt->deleteLater();
}

// ============================================================================
// leave a new eventloop
// ============================================================================
void  IcUiQmlApi  :: leaveEventLoop()
{
    if ( T_PrivPtr( m_obj )->evtLoopStack().isEmpty()) { return; }
    QEventLoop *evt = static_cast<QEventLoop*>( T_PrivPtr( m_obj )->evtLoopStack().pop());
    evt->quit();
}

// ============================================================================
// post the meta call
// ============================================================================
bool        IcUiQmlApi :: postMetaCall(
    QObject *obj, const QString &method,
    const QVariant & var1, const QVariant & var2,
    const QVariant & var3, const QVariant & var4
) {
    char method_name[64]; bool ret_call = false;
    if ( obj != Q_NULLPTR && ! method.isEmpty()) {
        const QChar *chr = method.constData( );
        int len = method.size(); len = ( len < 63 ? len : 62 );
        for ( int x = 0; x < len; x ++ ) { method_name[ x ] = chr[ x ].toLatin1(); }
        method_name[ len ] = 0;

        if ( var1.isNull()) { // no parameter
            ret_call = QMetaObject::invokeMethod( obj, & method_name[0], Qt::QueuedConnection );
        } else if ( var2.isNull() ) { // exist one parameter
            ret_call = QMetaObject::invokeMethod( obj, & method_name[0], Qt::QueuedConnection, Q_ARG( QVariant, var1 ));
        } else if ( var3.isNull() ) {
            ret_call = QMetaObject::invokeMethod( obj, & method_name[0], Qt::QueuedConnection, Q_ARG( QVariant, var1 ), Q_ARG( QVariant, var2 ));
        } else if ( var4.isNull() ) {
            ret_call = QMetaObject::invokeMethod( obj, & method_name[0], Qt::QueuedConnection, Q_ARG( QVariant, var1 ), Q_ARG( QVariant, var2 ), Q_ARG( QVariant, var3 ));
        } else {
            ret_call = QMetaObject::invokeMethod(
                obj, & method_name[0], Qt::QueuedConnection,
                Q_ARG( QVariant, var1 ), Q_ARG( QVariant, var2 ),
                Q_ARG( QVariant, var3 ), Q_ARG( QVariant, var4 )
            );
        }
    }
    return ret_call;
}



}

#endif
