#ifndef QXPACK_IC_DATATRANS_SVR_HXX
#define QXPACK_IC_DATATRANS_SVR_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/net/qxpack_ic_datatrans_buff.hxx"
#include <QByteArray>
#include <QObject>
#include <QStringList>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief the server of Data Transfer
 */
// ////////////////////////////////////////////////////////////////////////////
class IcDataTransHandler;
class QXPACK_IC_API  IcDataTransSvr : public QObject {
    Q_OBJECT
public:
    //! ctor
    explicit IcDataTransSvr (
        const QString &rs_type, const QString &host, quint16 port,
        IcDataTransFactory fact = Q_NULLPTR, void *fact_ctxt = Q_NULLPTR,
        QObject *pa = Q_NULLPTR
    );

    //! @20191031, nw added this ctor for outter thread...
    //! @note to spec. handler thread, override the createHandler() method
    IcDataTransSvr (
        const QString &rs_type, const QString &host, quint16 port,
        QThread *svr_thr,
        IcDataTransFactory fact = Q_NULLPTR, void *fact_ctxt = Q_NULLPTR,
        QObject *pa = Q_NULLPTR
    );

    //! dtor
    virtual ~IcDataTransSvr( ) Q_DECL_OVERRIDE;

    //! start server
    bool  startServer( );

    //! stop server
    void  stopServer( );

    //! check if this object is working
    bool  isListening( ) const;

    //! return current handler number
    bool  hasPendingHandler( ) const;

    //! get the current connection
    //! @note user should check the Tcp handler is working or not, \n
    //! some times, if remote client closed the connection, the hander is not working.
    QObject* takeNextPendingHandler( );

    //! while incoming new connection, the user call takeNextPendingHandler() to
    //! get the queued handler
    Q_SIGNAL void  newHandler( );

    //! error ocurred while accept new incoming
    Q_SIGNAL void  acceptError( int code, const QString &msg );

    //! return all supported rs type
    static QStringList avaliableRsTypeList( );

    //! return the full server name,  added by nw: 2019/12/13
    QString  fullSvrName( ) const;

protected:
    //! user can override it to get custom handler, default is  IcDataTransHandler
    //! @note the returned object can use delete or deleteLater() to destory
    virtual QObject*  createHandler(
        const QString &rs_type, qintptr dscr, IcDataTransFactory fact, void *fact_ctxt
    );

private:
    void   *m_obj;
    Q_DISABLE_COPY( IcDataTransSvr )
    friend class IcDataTransSvrPriv;
};


// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief the data transfer handler
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcDataTransHandler : public QObject {
    Q_OBJECT
public:
    //! dtor
    virtual ~IcDataTransHandler( ) Q_DECL_OVERRIDE;

    //! check if it is work
    //! @note user should check this again after taked from server.\n
    bool  isWorking( ) const;

    //! check the pending received data
    bool  hasPendingRecvData ( ) const;

    //! take next pending data from inner list
    bool  takeNextPendingRecvData ( QByteArray &data );

    //! write data into client
    bool  post ( const QByteArray &ba );

    //! this handler is finished, means remote closed or socket error
    //! @note user can check isWork() to determine object is still working or not. \n
    //! If object is not working, user can delete it.
    Q_SIGNAL void  finished();

    Q_SIGNAL void  newRecvData();

    Q_SIGNAL void  hostConnected( );

    Q_SIGNAL void  postDataSent();

    Q_SIGNAL void  errorMessage ( int, const QString &err_msg );

    //! ctor
    explicit IcDataTransHandler(
        const QString &rs_type,  qintptr sck_dscr,
        IcDataTransFactory fact = Q_NULLPTR, void *fact_ctxt = Q_NULLPTR,
        QObject *pa = Q_NULLPTR
    );

    //! ctor ( override )
    IcDataTransHandler(
        const QString &rs_type,  qintptr sck_dscr, QThread *thr,
        IcDataTransFactory fact = Q_NULLPTR, void *fact_ctxt = Q_NULLPTR,
        QObject *pa = Q_NULLPTR
    );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcDataTransHandler )
    friend class IcDataTransSvr;
    friend class IcDataTransSvrPriv;
};


}

#endif
