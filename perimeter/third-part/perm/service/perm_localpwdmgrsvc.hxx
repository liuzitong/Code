#ifndef PERM_LOCALPWDMGRSVC_HXX
#define PERM_LOCALPWDMGRSVC_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 本地密码管理
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API LocalPwdMgrSvc : public QxPack::IcSvcBase
{
    Q_OBJECT
    Q_PROPERTY(quint64 uid  READ uid  NOTIFY dataChanged)
    Q_PROPERTY(QString name READ name NOTIFY dataChanged)
    Q_PROPERTY(QString pwd  READ pwd  NOTIFY dataChanged)
    Q_PROPERTY(bool    rememberpwd  READ rememberpwd WRITE setRememberpwd  NOTIFY dataChanged)
    Q_PROPERTY(bool    autoLogin  READ autoLogin NOTIFY dataChanged)
public:
    static LocalPwdMgrSvc * getInstance( );
    static void release( );

    quint64 uid ( ) const;
    QString name( ) const;
    QString pwd ( ) const;
    bool    rememberpwd();
    bool    autoLogin();
    void    setRememberpwd( bool );

    void  loginTo( QString name, QString pwd);

    Q_SIGNAL void dataChanged( );

private:
    LocalPwdMgrSvc( );
    ~LocalPwdMgrSvc( );

private:
    void *m_obj;
    Q_DISABLE_COPY( LocalPwdMgrSvc )
};

}

#endif // PERM_LOCALPWDMGRSVC_HXX
