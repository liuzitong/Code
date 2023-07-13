#ifndef PERM_EDITABLEUSERPERMLISTINFO_CXX
#define PERM_EDITABLEUSERPERMLISTINFO_CXX

#include "perm_editableuserpermlistinfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( EditableUserPermListInfoPriv*, o )
class PERM_HIDDEN EditableUserPermListInfoPriv
{
private:
    EditableUserPermListInfo  * m_parent;
    QJsonArray                     m_permList;

public:
    EditableUserPermListInfoPriv ( EditableUserPermListInfo * );
    ~EditableUserPermListInfoPriv( );

    inline QJsonArray & permList( ) { return m_permList; }
};

EditableUserPermListInfoPriv::EditableUserPermListInfoPriv(EditableUserPermListInfo *pa)
{
    m_parent = pa;
}

EditableUserPermListInfoPriv::~EditableUserPermListInfoPriv()
{

}

EditableUserPermListInfo::EditableUserPermListInfo()
{
    m_obj = perm_new( EditableUserPermListInfoPriv, this );
}

EditableUserPermListInfo::~EditableUserPermListInfo()
{
    perm_delete( m_obj, EditableUserPermListInfoPriv );
}

QJsonArray EditableUserPermListInfo::permList() const
{ return T_PrivPtr( m_obj )->permList(); }

void EditableUserPermListInfo::addPerm(const bool &isInherit, const bool &isPriv, const bool &isEnable, const QString &perm)
{ T_PrivPtr( m_obj )->permList().push_back(QJsonObject({{"isInherit", isInherit}, {"isPriv", isPriv}, {"isEnable", isEnable}, {"perm", perm}})); }

}

#endif // PERM_EDITABLEUSERPERMLISTINFO_CXX
