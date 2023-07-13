#ifndef PERM_PERMBROKER_HXX
#define PERM_PERMBROKER_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

class UserListInfo;
class UserAndPermInfo;
class SpecUserPermListInfo;
class SpecUserGrpListInfo;
class EditableUserInfo;
class UserGrpListInfo;
class SpecUserGrpPermListInfo;
class EditableUserGrpInfo;
class EditableUserDiChgPwdInfo;
class UserInfoData;
class UserGroupInfoData;
// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Rpc保持者
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API PermBroker : public QxPack::IcSvcBase
{
    Q_OBJECT
private:
    PermBroker( );
    ~PermBroker( );

public:
    static PermBroker * getInstance( );
    static void freeInstance( );
    void  connectToServer( const QString & );
    void  disconnectToServer( );

    UserAndPermInfo * userAndPermInfo( ) const;
    bool  login( const QJsonObject & );
    bool  logout( );
    void  getUserList( UserListInfo & );
    bool  deleteUser ( UserListInfo & );
    bool  getSpecUserPermList( const UserListInfo &, SpecUserPermListInfo & );
    bool  getSpecUserGrpList ( const UserListInfo &, SpecUserGrpListInfo  & );
    void  getUserGrpList( UserGrpListInfo & );
    bool  deleteUserGrp( UserGrpListInfo & );
    bool  getSpecUserGrpPermList( const UserGrpListInfo &, SpecUserGrpPermListInfo & );
    bool  getSpecUserGrpPermList( const QList<quint64> &, QStringList & );
    bool  createUser( EditableUserInfo &, UserListInfo & );
    bool  createUserGrp( EditableUserGrpInfo &, UserGrpListInfo & );
    bool  setUser( EditableUserInfo &, UserListInfo & );
    bool  setUserGrp( EditableUserGrpInfo &, UserGrpListInfo & );
    bool  getUser( const UserListInfo &, EditableUserInfo & );
    bool  getUser( const UserListInfo &, UserInfoData & );
    bool  getUserGrp( const UserGrpListInfo &, EditableUserGrpInfo & );
    bool  getUserGrp( const UserGrpListInfo &, UserGroupInfoData & );
    bool  diChgUserPwd( EditableUserDiChgPwdInfo & );
    void  quitRpcPerm();

    Q_SIGNAL void error( int , const QString & );
    Q_SIGNAL void ready( );
    Q_SIGNAL void quit( );

private:
    void *m_obj;
    Q_DISABLE_COPY( PermBroker )
};

}

#endif // PERM_PERMBROKER_HXX
