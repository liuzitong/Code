#ifndef JRPCPLAT_LOCCLI_HXX
#define JRPCPLAT_LOCCLI_HXX

#include "../common/jrpcplat_def.h"
#include "../base/jrpcplat_package.hxx"
#include "../clicfg/jrpcplat_locclicfg.hxx"
#include <QObject>
#include <QString>

namespace JRpcPlat {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  the client of the JRpcPlat
 */
// ////////////////////////////////////////////////////////////////////////////
class JRPCPLAT_API  LocCli : public QObject {
    Q_OBJECT
public:
    //! create an client
    explicit LocCli( const LocCliCfg & );

    //! dtor
    virtual ~LocCli() Q_DECL_OVERRIDE;

    // ========================================================================
    // connection API
    // ========================================================================
    //! connect to host, this is an async-method
    //! @param svr_name  [in] the server name, here is the short server name.
    //! @note use serverFullName() to get platform spec. full name. \n
    //!  while connected to server, the signal serverConnected() will be emited, user can  \n
    //!  use it to do regMethod() or other operation.
    //! @warning if server could not be connected, all APIs depend on server will be failed.
    Q_SLOT  void        connectToServer( const QString &svr_name );

    //! disconnect from host
    Q_SLOT  void        disconnectFromServer();

    //! check if currently it is connected to server
    Q_SLOT  bool        isWorking() const;

    //! full path with name
    //! @warning this API need server connected
    Q_SLOT  QString     serverFullName() const;

    //! register client method on server
    //! @details the method name is like a topic on the server
    //! @warning this API need server connected
    Q_SLOT  bool        regMethod ( const QStringList & );

    //! remove method from server
    //! @details the method name is like a topic on the server
    //! @warning this API need server connected
    Q_SLOT  bool        unregMethod( const QStringList & );

    //! get back the unique string for this client
    //! @since  0.2.0.0
    Q_SLOT  QString     uidStr( ) const;

    // ========================================================================
    // this client --> other client
    // ========================================================================
    //! post a notice
    //! @return true means notice has been posted.
    //! @note if the server is not connected, do nothing. And this API is not a \n
    //! blocking API.
    Q_SLOT  bool        postNotice( const NoticePkg & );

    //! invoke the request
    //! @param  req      [in] the request package
    //! @param  req_evt  [in] if it is true, will make a eventloop in this method, so \n
    //! caller should not be blocked ( exclusive user input )
    //! @param  wait_ms  [in] the million seconds to wait. ( 0 means ignore, -1 means infinite ) \n
    //! @return this method will wait until time-out. If remote has been sovled the request, \n
    //! the response package should not be empty, otherwise it will be an empty response.
    Q_SLOT  ResponsePkg invokeRequest( const RequestPkg &req, bool req_evt, int wait_ms = -1 );

    //! post binbcst
    //! @since  0.4.0
    //! @return true means binbcst has been posted.
    //! @note if the server is not connected, do nothing. And this API is not a \n
    //! blocking API.
    Q_SLOT  bool        postBinBcst( const BinBcstPkg & );

    // ========================================================================
    // other client -> this client
    // ========================================================================
    //! post the response for remote request
    //! @return true means posted.
    Q_SLOT  bool        postResponse( const ResponsePkg & );

    //! take pending request
    Q_SLOT  RequestPkg  takeNextPendingRequest();

    //! take pending notice
    Q_SLOT  NoticePkg   takeNextPendingNotice();

    //! take pending binbcst
    Q_SLOT  BinBcstPkg  takeNextPendingBinBcst();

    // ========================================================================
    // signals
    // ========================================================================
    Q_SIGNAL void  newRequest ( );
    Q_SIGNAL void  newNotice ( );
    Q_SIGNAL void  serverConnected( );    // remote server has been connected
    Q_SIGNAL void  serverDisconnected( ); // remote closed or error ocurred

    //! @since  0.2.0.0
    Q_SIGNAL void  ready();               // this client has been ready for use ( this signal after the serverConnected )

    //! @since 0.4.0
    Q_SIGNAL void  newBinBcst( );


protected:
    ResponsePkg   ivkReqSem( const RequestPkg &, int wait_ms = -1, bool is_spec = false );
    Q_SLOT  bool  ivkReqEvt( void *ivk_sp, const JRpcPlat::RequestPkg &, bool is_spec = false );


public :
    // appended..
    //! query spec. method list is online or not
    //! @since 0.5.0
    //! @note the response.data() returned an array like [ true, false, true ...] \n
    //!   each element means a spec. position method in the 'me_lst'
    //! @example \n
    //!   me_lst is [ "methodA", "methodB" ]. \n
    //!   returned array is [ true, false ]   \n
    //!   that means: Currently, "methodA" is online, but "methodB" is offline.
    Q_SLOT  ResponsePkg invokeQuery( const QStringList &me_lst, bool req_evt, int wait_ms = -1 );
private:
    void *m_obj;
    Q_DISABLE_COPY( LocCli )
};


}

#endif
