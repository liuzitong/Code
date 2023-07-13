#ifndef PERM_EDITABLEUSERDICHGPWDINFO_CXX
#define PERM_EDITABLEUSERDICHGPWDINFO_CXX

#include "perm_editableuserdichgpwdinfo.hxx"
#include "perm/model/perm_passwordvalidator.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( EditableUserDiChgPwdInfoPriv*, o )
class PERM_HIDDEN EditableUserDiChgPwdInfoPriv
{
private:
    EditableUserDiChgPwdInfo  * m_parent;
    PasswordValidator           m_pwdValidator;
    QString    m_name, m_oldPwd, m_newPwd;

public:
    explicit EditableUserDiChgPwdInfoPriv ( EditableUserDiChgPwdInfo *pa );
    ~EditableUserDiChgPwdInfoPriv( );

    inline QObject * pwdValidatorObj() { return &m_pwdValidator; }
    inline QString & name   ( ) { return m_name;   }
    inline QString & oldPwd ( ) { return m_oldPwd; }
    inline QString & newPwd ( ) { return m_newPwd; }
};

EditableUserDiChgPwdInfoPriv::EditableUserDiChgPwdInfoPriv(EditableUserDiChgPwdInfo *pa)
{
    m_parent = pa;
}

EditableUserDiChgPwdInfoPriv::~EditableUserDiChgPwdInfoPriv()
{

}

EditableUserDiChgPwdInfo::EditableUserDiChgPwdInfo()
{
    m_obj = perm_new( EditableUserDiChgPwdInfoPriv, this );
}

EditableUserDiChgPwdInfo::~EditableUserDiChgPwdInfo()
{
    perm_delete( m_obj, EditableUserDiChgPwdInfoPriv );
}

QObject *EditableUserDiChgPwdInfo::pwdValidatorObj() const
{ return T_PrivPtr( m_obj )->pwdValidatorObj(); }

QString EditableUserDiChgPwdInfo::name() const
{ return T_PrivPtr( m_obj )->name(); }

QString EditableUserDiChgPwdInfo::oldPwd() const
{ return T_PrivPtr( m_obj )->oldPwd(); }

QString EditableUserDiChgPwdInfo::newPwd() const
{ return T_PrivPtr( m_obj )->newPwd(); }

void EditableUserDiChgPwdInfo::setUserAndPwd(const QJsonObject &userAndPwd)
{
    if ( userAndPwd.contains("name") )
    { T_PrivPtr( m_obj )->name() = userAndPwd.value("name").toString(); }
    if ( userAndPwd.contains("oldPwd") )
    { T_PrivPtr( m_obj )->oldPwd() = userAndPwd.value("oldPwd").toString(); }
    if ( userAndPwd.contains("newPwd") )
    { T_PrivPtr( m_obj )->newPwd() = userAndPwd.value("newPwd").toString(); }
}

void EditableUserDiChgPwdInfo::clear()
{
    T_PrivPtr( m_obj )->name()   = QString(); emit nameChanged();
    T_PrivPtr( m_obj )->newPwd() = QString(); emit newPwdChanged();
    T_PrivPtr( m_obj )->oldPwd() = QString(); emit oldPwdChanged();
}

}

#endif // PERM_EDITABLEUSERDICHGPWDINFO_CXX
