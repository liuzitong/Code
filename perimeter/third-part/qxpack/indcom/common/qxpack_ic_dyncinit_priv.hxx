#ifndef QXPACK_IC_DYNCINIT_PRIV_HXX
#define QXPACK_IC_DYNCINIT_PRIV_HXX

#include <qxpack/indcom/common/qxpack_ic_def.h>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// according to c++ rules:
// 1) .bss segment initalized after program loaded
//     POD variable will stored in .bss
// 2) the static variable defined in function will
//    initalized while fisrt call this function.
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcDyncInit {
public:
    typedef void (* Handler )( );
    IcDyncInit ( Handler cr, Handler del )
    {
        m_cr_func = cr; m_del_func = del;
        if ( m_cr_func != nullptr ) { (* m_cr_func )(); }
    }
    ~IcDyncInit( )
    {
        if ( m_del_func != nullptr ){ (* m_del_func )(); }
    }

private:
    Handler m_cr_func, m_del_func;
};


}

#endif
