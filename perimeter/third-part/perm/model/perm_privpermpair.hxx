#ifndef PERM_PRIVPERMPAIR_HXX
#define PERM_PRIVPERMPAIR_HXX

#include <QString>
#include "perm/common/perm_def.h"

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//
// 私有权限描述
//
// ////////////////////////////////////////////////////////////////////////////
class PERM_API PrivPermPair
{
public:
    PrivPermPair();
    PrivPermPair ( const PrivPermPair & );
    PrivPermPair &  operator = ( const PrivPermPair & );
    virtual ~PrivPermPair( );

    QString  permName( ) const;
    bool     isAllow( ) const;

    void  setPermName( const QString & );
    void  setIsAllow( const bool & );

private:
    void *m_obj;
};

}


#endif // PERM_PRIVPERMPAIR_HXX
