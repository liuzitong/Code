// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_msgbus
  @author  night wing
  @date    2019/1, 2019/04, 2019/05
  @version 0.1.0.0
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_MSGBUS_HXX
#define QXPACK_IC_MSGBUS_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QJsonObject>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief message package
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcMsgBusPkg {
public:
    //! create empty package
    IcMsgBusPkg( );

    //! create by another
    IcMsgBusPkg( const IcMsgBusPkg & );

    //! copy assign
    IcMsgBusPkg & operator = ( const IcMsgBusPkg & );

    //! dtor
    virtual ~IcMsgBusPkg( );

    void  setGroupName( const QString &nm );
    void  setMessage  ( const QString &msg );
    void  setJoParam  ( const QJsonObject &jo );

    QString  groupName( ) const;
    QString  message( )  const;
    QJsonObject  joParam( ) const;

private:
    void *m_obj;
};

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief a QML view access wrapper, just for read only
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcMsgBusPkgQObj : public QObject {
    Q_OBJECT
    Q_PROPERTY( QString  groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged )
    Q_PROPERTY( QString  message   READ message   WRITE setMessage   NOTIFY messageChanged   )
    Q_PROPERTY( QJsonObject  joParam READ joParam WRITE setJoParam   NOTIFY joParamChanged   )
public :
    explicit IcMsgBusPkgQObj ( QObject *pa = Q_NULLPTR );
    virtual ~IcMsgBusPkgQObj ( );

    void  setContent( IcMsgBusPkg &pkg );
    IcMsgBusPkg   content( ) const;

    QString      groupName( ) const;
    QString      message( )   const;
    QJsonObject  joParam( )   const;

    void  setGroupName( const QString & );
    void  setMessage  ( const QString & );
    void  setJoParam  ( const QJsonObject & );

    Q_SIGNAL void  groupNameChanged( );
    Q_SIGNAL void  messageChanged( );
    Q_SIGNAL void  joParamChanged( );

private:
    IcMsgBusPkg  m_pkg;
    void *m_ext;
    Q_DISABLE_COPY( IcMsgBusPkgQObj )
};


// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Message Bus
 * @details \n
 *    the message bus is used for post message between every service
 * @note  the message bus used QT5 signal & slot inner.
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcMsgBus : public QObject {
    Q_OBJECT
public:
    //! CTOR
    IcMsgBus ( QObject *pa = Q_NULLPTR );

    //! DTOR
    virtual ~IcMsgBus ( );

    /*!
     * @brief add listener about a topic
     * @param obj   [in] the object
     * @param group [in] the group name string
     * @note  the 'obj' must has below members:
     *     Q_SLOT void  IcMsgBus_onMsg ( const IcMsgBusPkg &pkg )
     */
    Q_INVOKABLE bool    add( QObject *obj, const QString &group );

    /*!
     * @brief explicit remove the object from group
     * @param obj   [in] the object that added in the group
     * @param group [in] the group name
     * @warning if user delete the object before, it will auto-remove \n
     *    the object from group.
     */
    Q_INVOKABLE bool    rmv( QObject *obj, const QString &group );

    /*!
     * @brief post a message to group
     * @param pkg [in] the message package
     * @note after posted, the pkg can be deleted \n
     */
    Q_INVOKABLE void    post( const IcMsgBusPkg &pkg );

    /*!
     * @brief post a message to group ( override )
     * @param grp [ in ] the group name
     * @param msg [ in ] simple message
     * @param jo  [ in ] the json object
     * @param va  [ opt] the QVariantList() optional
     */
    Q_INVOKABLE void    post(
        const QString &grp, const QString &msg, const QJsonObject &jo, const QVariantList &va
    );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcMsgBus )
};

}

Q_DECLARE_METATYPE( QxPack::IcMsgBusPkg )

#endif
