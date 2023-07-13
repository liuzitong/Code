#ifndef PERM_EDITABLEUSERGRPPERMLISTINFO_CXX
#define PERM_EDITABLEUSERGRPPERMLISTINFO_CXX

#include "perm_editableusergrppermlistinfo.hxx"
#include "perm/common/perm_memcntr.hxx"
#include <QJsonObject>

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( EditableUserGrpPermListInfoPriv*, o )
class PERM_HIDDEN EditableUserGrpPermListInfoPriv
{
private:
    EditableUserGrpPermListInfo  * m_parent;
    QJsonArray                     m_permInfoList;

public:
    EditableUserGrpPermListInfoPriv ( EditableUserGrpPermListInfo * );
    ~EditableUserGrpPermListInfoPriv( );

    inline QJsonArray & permInfoList( ) { return m_permInfoList; }
};

EditableUserGrpPermListInfoPriv::EditableUserGrpPermListInfoPriv(EditableUserGrpPermListInfo *pa)
{
    m_parent = pa;
}

EditableUserGrpPermListInfoPriv::~EditableUserGrpPermListInfoPriv()
{

}

EditableUserGrpPermListInfo::EditableUserGrpPermListInfo()
{
    m_obj = perm_new( EditableUserGrpPermListInfoPriv, this );
}

EditableUserGrpPermListInfo::~EditableUserGrpPermListInfo()
{
    perm_delete( m_obj, EditableUserGrpPermListInfoPriv );
}

QJsonArray EditableUserGrpPermListInfo::permInfoList() const
{ return T_PrivPtr( m_obj )->permInfoList(); }

void EditableUserGrpPermListInfo::addPerm(const bool &isSelected, const QString &permName)
{ T_PrivPtr( m_obj )->permInfoList().push_back(QJsonObject({{"isSelected", isSelected}, {"permName", permName}})); }

}


#endif // PERM_EDITABLEUSERGRPPERMLISTINFO_CXX
