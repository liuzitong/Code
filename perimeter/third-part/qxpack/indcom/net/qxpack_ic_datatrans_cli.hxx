#ifndef QXPACK_IC_DATATRANS_CLI_HXX
#define QXPACK_IC_DATATRANS_CLI_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/net/qxpack_ic_datatrans_buff.hxx"
#include <QObject>
#include <QString>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief the data transfer client
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcDataTransCli : public QObject {
    Q_OBJECT
public:
    //! ctor
    explicit IcDataTransCli (
        const QString  &rs_type,
        IcDataTransFactory  fact = Q_NULLPTR, void *fact_ctxt = Q_NULLPTR,
        QObject *pa = Q_NULLPTR
    );

    //! @20191031, nw added
    IcDataTransCli (
        const QString  &rs_type, QThread *thr,
        IcDataTransFactory  fact = Q_NULLPTR, void *fact_ctxt = Q_NULLPTR,
        QObject *pa = Q_NULLPTR
    );

    //! dtor
    virtual ~IcDataTransCli ( ) Q_DECL_OVERRIDE;

    //! check it is work
    bool  isWorking( ) const;

    //! @note if connected, the hostConnected() signal is emitting
    //! @note to disconnectFromHost(), directly delete this object.
    void  connectToHost ( const QString &, quint16 port );

    //! check if exist pending recvieved data
    bool  hasPendingRecvData ( );

    //! take first response
    bool  takeNextPendingRecvData( QByteArray & );

    //! sending data, false means failed
    bool  post( const QByteArray & );

    //! full host name. @20191024, nw added
    QString  fullHostName( ) const;

    //! a new data arrived
    Q_SIGNAL void  newRecvData( );

    //! data sent
    Q_SIGNAL void  postDataSent( );

    //! if remote connected
    Q_SIGNAL void  hostConnected( );

    //! if remote closed socket, or network error
    Q_SIGNAL void  finished( );

    //! if raised error
    Q_SIGNAL void  errorMessage( int err, const QString &err_msg );

    // all rs type list
    static QStringList avaliableRsTypeList( );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcDataTransCli )
};

}

#endif
