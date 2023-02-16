#ifndef QXPACK_IC_DATATRANS_RSHDEBASE_HXX
#define QXPACK_IC_DATATRANS_RSHDEBASE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/net/qxpack_ic_datatrans_buff.hxx"
#include <QObject>
#include <QSharedPointer>
#include <QIODevice>
#include <QStringList>
#include <QVariant>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// base of the data trans implement
// ////////////////////////////////////////////////////////////////////////////
class IcDataTransRsHdeBasePriv;
class QXPACK_IC_API  IcDataTransRsHdeBase : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcDataTransRsHdeBase )
protected:
    IcDataTransRsHdeBasePriv *m_base_priv;
protected:
            IcDataTransPkgReadBuff*  pkgReadBuffer ( );
            IcDataTransPkgWriteBuff* pkgWriteBuffer( );
    Q_SLOT  void  createBuffer ( QSharedPointer<QIODevice>& );
    Q_SLOT  void  deleteBuffer ( );
    Q_SLOT virtual void  readBuff_onNewPackage  ( const QByteArray & );
    Q_SLOT virtual void  writeBuff_onPackageSent( );
public :
    //! ctor
    explicit IcDataTransRsHdeBase(
        IcDataTransFactory fact = Q_NULLPTR, void *fact_ctxt = Q_NULLPTR,
        QObject *pa = Q_NULLPTR
    );

    //! dtor
    virtual ~IcDataTransRsHdeBase( ) Q_DECL_OVERRIDE;

    //! current socket type
    Q_INVOKABLE virtual QString  rsType( ) const;

    //! check if it is working
    Q_INVOKABLE virtual bool     isWorking( ) const;

    //! open the target host by string
    Q_SLOT virtual void open (
        const QString &host_name, quint16 port, int open_mode
    );

    //! initailize by descriptor value
    Q_SLOT virtual bool  initByDscr ( QVariant );

    //! close by current socket
    Q_SLOT virtual void  close( );

    //! post the bytearray data
    //! @param  ba     [in] the data
    //! @param  is_pkg [in] the package data will emit dataSent(), otherwise not
    Q_SLOT virtual bool  post ( const QByteArray &ba, bool is_pkg = true );

    //! full host name  @2019/10/24 nw added
    Q_SLOT virtual QString  fullHostName( ) const;

    //! has new data arrived
    Q_SIGNAL void  newData ( const QByteArray& );

    //! data has been sent
    Q_SIGNAL void  dataSent( );

    //! host connected
    Q_SIGNAL void  hostConnected( );

    //! remote finished.
    Q_SIGNAL void  finished ( );

    //! some error ocurred.
    Q_SIGNAL void  errorMsg ( int err, const QString& );

    // ========================================================================
    // static builder
    // ========================================================================
    //! @brief create object by Rs Type
    //! @param  rs_type    [in]     the rs type string name
    //! @param  fact       [opt_in] the buffer factory
    //! @param  fact_ctxt  [opt_in] the buffer factory context
    //! @param  pa         [opt_in] the parent object
    //! @return if 'rs_type' existed, create new object
    static IcDataTransRsHdeBase*  createObj (
        const QString &rs_type,
        IcDataTransFactory fact = Q_NULLPTR, void *fact_ctxt = Q_NULLPTR,
        QObject *pa = Q_NULLPTR
    );

    //! @brief delete the object created by createObj()
    static void  deleteObj( IcDataTransRsHdeBase* );

    //! @return current installed rs type list
    static QStringList  avaliableRsTypeList( );

};


}

#endif
