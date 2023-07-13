#ifndef PERM_TYPEDEF_HXX
#define PERM_TYPEDEF_HXX

#include <QList>
#include <QString>
#include "perm/common/perm_def.h"

namespace FcPerm {


class GroupSimpleDesc;
typedef QList<GroupSimpleDesc> AuthGroupList;   // 用户可以给其他用户授权的用户组列表

typedef QList<QString> AuthPermList;            // 用户可以给其他用户授权的列表

typedef QList<QString> ActualPermList;          // 私有权限与组合权限合成的最终权限列表

typedef QList<QString> GroupPermList;           // 用户组权限列表

class PrivPermPair;
typedef QList<PrivPermPair> PrivPermList;       // 私有权限列表

typedef QList<GroupSimpleDesc> GroupSimpleList; // 用户组列表

}
#endif // PERM_TYPEDEF_HXX
