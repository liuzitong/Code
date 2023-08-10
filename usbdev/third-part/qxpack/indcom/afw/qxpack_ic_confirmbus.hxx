// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_confirmbus
  @author  night wing
  @date    2019/4, 2019/05, 2019/11
  @version 0.1.2.0, 0.2.0.0
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_CONFIRMBUS_HXX
#define QXPACK_IC_CONFIRMBUS_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QJsonObject>
#include <QSemaphore>
#include <QEventLoop>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief confirm package
 * @note  the confirm package inner is PImpl, but shared one context !
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcConfirmBusPkg {
public:
    //! create empty package
    IcConfirmBusPkg( );

    //! create by another
    IcConfirmBusPkg( const IcConfirmBusPkg & );

    //! copy assign
    IcConfirmBusPkg & operator = ( const IcConfirmBusPkg & );

    //! dtor
    virtual ~IcConfirmBusPkg( );

    //! check if it is has been got result
    bool  isGotResult ( ) const;

    //! check if it is empty
    bool  isEmpty( ) const;

    //! access the group name
    QString  groupName( ) const;
    void     setGroupName( const QString & );

    //! access the message text
    QString  message( )   const;
    void     setMessage ( const QString & );

    //! access the jo-parameter
    QJsonObject  joParam( ) const;
    void      setJoParam ( const QJsonObject & );

    //! access the result
    QJsonObject  result ( ) const;
    void      setResult  ( const QJsonObject &jo );

    //! @warning after this call, the confirmbus will wait until setResult() or
    //! if this package is previsouly grabbed by UI, it return false.
    bool  grab ( );

protected:
    // check if grabbed
    bool  isGrabbed( ) const;

    // setup eventloop ptr
    void  setEvtLoop( QEventLoop* );

    // create or get current wait sem..
    QSemaphore*  ensureWaitSem( );

    void  addCbIfSetRsl ( void (* cb )(void*, const QJsonObject & ), void *ctxt );
    void  rmvCbIfSetRsl ( void (* cb )(void*, const QJsonObject & ), void *ctxt );

    friend class IcConfirmBus;
    friend class IcConfirmBusPriv;
    friend class IcConfirmBusPkgQObj;
private:
    void *m_obj;
};




// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief a QML view access wrapper, just for read only
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcConfirmBusPkgQObj : public QObject {
    Q_OBJECT
    Q_PROPERTY( QString      groupName   READ groupName WRITE setGroupName NOTIFY groupNameChanged )
    Q_PROPERTY( QString      message     READ message   WRITE setMessage   NOTIFY messageChanged   )
    Q_PROPERTY( QJsonObject  joParam     READ joParam   WRITE setJoParam   NOTIFY joParamChanged   )
    Q_PROPERTY( QJsonObject  result      READ result    WRITE setResult    NOTIFY resultChanged    )
    Q_PROPERTY( bool     isGotResult     READ isGotResult                  NOTIFY isGotResultChanged  )
public :
    //! @deprecated using newInst() instead..
    explicit  IcConfirmBusPkgQObj ( QObject * = Q_NULLPTR );

    virtual ~IcConfirmBusPkgQObj ( ) Q_DECL_OVERRIDE;

    void  setContent( const IcConfirmBusPkg &pkg );
    IcConfirmBusPkg  content( ) const;

    QString      groupName( ) const;
    QString      message( )   const;
    QJsonObject  joParam( )   const;
    QJsonObject  result( )    const;
    bool         isGotResult( ) const;

    void  setGroupName( const QString & );
    void  setMessage  ( const QString & );
    void  setJoParam  ( const QJsonObject & );
    void  setResult   ( const QJsonObject & );

    Q_INVOKABLE bool  grab( );

    Q_SIGNAL void  groupNameChanged( );
    Q_SIGNAL void  messageChanged( );
    Q_SIGNAL void  joParamChanged( );
    Q_SIGNAL void  resultChanged ( );
    Q_SIGNAL void  isGotResultChanged( );

    //! make sure the create a object
    //! @note this object will be autodeleted after confirm package called setResult()
    static IcConfirmBusPkgQObj *newInst( const IcConfirmBusPkg & );

protected:
    //! ctor
    explicit IcConfirmBusPkgQObj( const IcConfirmBusPkg & );

    static void  handleRslCb( void*, const QJsonObject & );

private:
    IcConfirmBusPkg  m_pkg;
    void *m_ext;
    Q_DISABLE_COPY( IcConfirmBusPkgQObj )
};




// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Confirm Bus
 * @details \n
 *    the confirm bus is used for submit a message and wait it has been sovled
 *  by User ( UI )
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcConfirmBus : public QObject {
    Q_OBJECT
public:
    //! ctor
    IcConfirmBus ( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcConfirmBus ( ) Q_DECL_OVERRIDE;

    /*!
     * @brief add listener about a group name
     * @param obj   [in] the object
     * @param group [in] the group name string
     * @warning the object must be UI thread object.
     * @note  the 'obj' must has below members:
     *     Q_SLOT void  IcConfirmBus_onReqConfirm ( QxPack::IcConfirmBusPkg &pkg )
     * @note  The listener is placed at first head in the list, and the confirm
     *     will be called one by one test. Once a listener setuped result ( empty or has data ),
     *     then finish the confirm process.  
     */
    Q_INVOKABLE bool   add( QObject *obj, const QString &group );

    /*!
     * @brief explicit remove the object from group
     * @param obj   [in] the object that added in the group
     * @param group [in] the group name
     * @warning if user delete the object before, it will auto-remove \n
     *    the object from group.
     */
    Q_INVOKABLE bool   rmv( QObject *obj, const QString &group );

    /*!
     * @brief confirm the information 
     * @param pkg [in] the confirm message package
     * @note  the confirm will be blocked until package got a result
     */
    Q_INVOKABLE void   reqConfirm( QxPack::IcConfirmBusPkg &pkg );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcConfirmBus )
};

}

Q_DECLARE_METATYPE( QxPack::IcConfirmBusPkg )

#endif
