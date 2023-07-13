#ifndef PERM_DYNCINIT_PRIV_HXX
#define PERM_DYNCINIT_PRIV_HXX

#include "perm_def.h"

namespace FcPerm {

// ///////////////////////////////////////////////////////////////
// according to c++ rules:
// 1) .bss segment initalized after program loaded
//     POD variable will stored in .bss
// 2) the static variable defined in function will
//    initalized while fisrt call this function.
// ///////////////////////////////////////////////////////////////
class PERM_HIDDEN  DyncInit {
public:
    typedef void (* Handler )( );
    DyncInit ( Handler cr, Handler del )
    {
        m_cr_func = cr; m_del_func = del;
        if ( m_cr_func != nullptr ) { (* m_cr_func )(); }
    }
    ~DyncInit( )
    {
        if ( m_del_func != nullptr ){ (* m_del_func )(); }
    }

private:
    Handler m_cr_func, m_del_func;
};


}

#endif
