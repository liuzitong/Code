#ifndef QXPACK_IC_DATATRANS_RSSVRBASE_HXX
#define QXPACK_IC_DATATRANS_RSSVRBASE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/net/qxpack_ic_datatrans_buff.hxx"
#include <QObject>
#include <QIODevice>
#include <QStringList>
#include <QList>
#include <QVariant>

namespace QxPack {

class IcDataTransRsHdeBase;

// ////////////////////////////////////////////////////////////////////////////
// base of the data trans socket implement
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcDataTransRsSvrBase : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcDataTransRsSvrBase )
public :
    explicit IcDataTransRsSvrBase ( QObject *pa = Q_NULLPTR );

    virtual ~IcDataTransRsSvrBase ( ) Q_DECL_OVERRIDE;

    Q_INVOKABLE virtual bool isListening( ) const;

    Q_INVOKABLE virtual bool startServer( const QString &nm, quint16 port );

    Q_INVOKABLE virtual void stopServer( );

    Q_INVOKABLE virtual QString  rsType( ) const;

    Q_INVOKABLE virtual quint16  port( ) const;

    // @2019/10/24 nw added
    Q_INVOKABLE virtual QString  fullServerName( ) const;

    Q_SIGNAL void  reqHandleNewIncoming( QVariant );

    Q_SIGNAL void  acceptError( int, const QString & );

    // ========================================================================
    // static builder
    // ========================================================================
    //! @brief create object by Rs Type
    //! @param  rs_type    [in]     the rs type string name
    //! @param  pa         [opt_in] the parent object
    //! @return if 'rs_type' existed, create new object
    static IcDataTransRsSvrBase*  createObj (
        const QString &rs_type, QObject *pa = Q_NULLPTR
    );

    //! @brief delete the object created by createObj()
    static void  deleteObj( IcDataTransRsSvrBase* );

    //! @return current installed rs type list
    static QStringList  avaliableRsTypeList( );
};


}

#endif
