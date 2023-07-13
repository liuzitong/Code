#ifndef PERM_EDITABLEUSERGRPLISTINFO_CXX
#define PERM_EDITABLEUSERGRPLISTINFO_CXX

#include "perm_editableusergrplistinfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( EditableUserGrpListInfoPriv*, o )
class PERM_HIDDEN EditableUserGrpListInfoPriv
{
private:
    EditableUserGrpListInfo  * m_parent;
    QJsonArray m_grpList;

public:
    EditableUserGrpListInfoPriv ( EditableUserGrpListInfo * );
    ~EditableUserGrpListInfoPriv( );

    inline QJsonArray & grpList( ) { return m_grpList; }
};

EditableUserGrpListInfoPriv::EditableUserGrpListInfoPriv(EditableUserGrpListInfo *pa)
{
    m_parent = pa;
}

EditableUserGrpListInfoPriv::~EditableUserGrpListInfoPriv()
{

}

EditableUserGrpListInfo::EditableUserGrpListInfo()
{
    m_obj = perm_new( EditableUserGrpListInfoPriv, this );
}

EditableUserGrpListInfo::~EditableUserGrpListInfo()
{
    perm_delete( m_obj, EditableUserGrpListInfoPriv );
}

QJsonArray EditableUserGrpListInfo::grpList() const
{ return T_PrivPtr( m_obj )->grpList(); }

void EditableUserGrpListInfo::addGrp(const bool &isSelected, const QString &gid, const QString &name)
{ T_PrivPtr( m_obj )->grpList().push_back( QJsonObject({{"isSelected", isSelected}, {"gid", gid}, {"name", name}}) ); }

}

#endif // PERM_EDITABLEUSERGRPLISTINFO_CXX
