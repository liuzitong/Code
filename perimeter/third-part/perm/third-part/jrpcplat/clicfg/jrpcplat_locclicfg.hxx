#ifndef JRPCPLAT_LOCCLICFG_HXX
#define JRPCPLAT_LOCCLICFG_HXX

#include "../common/jrpcplat_def.h"
#include <QString>
#include <QJsonObject>

namespace JRpcPlat {

// ////////////////////////////////////////////////////////////////////////////
//
//                      the local client config.
// {
//    "version"   : "1.0",
//    "accToken"  : "abcdef"
//    "aliasName" : "aliasName"
// }
// ////////////////////////////////////////////////////////////////////////////
class JRPCPLAT_API  LocCliCfg {
public:
    //! an empty server config.
    explicit LocCliCfg( );

    //! ctor by json object
    LocCliCfg ( const QJsonObject & );

    //! ctor by file name
    LocCliCfg( const QString &fn );

    //! create with other
    LocCliCfg ( const LocCliCfg & );

    //! assign copy
    LocCliCfg &   operator = ( const LocCliCfg & );

#ifdef JRPCPLAT_CXX11
    LocCliCfg ( LocCliCfg && );
    LocCliCfg &   operator = ( LocCliCfg && );
#endif

    //! dtor
    ~LocCliCfg( );


    //! check if it is empty
    bool                isEmpty() const;

    //! access the access token
    QString             accToken() const;
    void              setAccToken( const QString & );

    //! access the alias name
    QString             aliasName() const;
    void              setAliasName( const QString& );

private:
    void *m_obj;
};


}

#endif
