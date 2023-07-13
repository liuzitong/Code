#ifndef PERM_PERM_MOD_HXX
#define PERM_PERM_MOD_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_modmgrbase.hxx"
#include <QObject>
#include <QQmlEngine>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//
// the perm module
//
// ////////////////////////////////////////////////////////////////////////////
class PERM_API PermMod : public QxPack::IcModMgrBase {
    Q_OBJECT
public :
    //! ctor
    explicit PermMod ( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~PermMod( ) Q_DECL_OVERRIDE;

    //! register types
    static void registerTypesEx( QxPack::IcAppCtrlBase* );

    //! register Base types to QQmlEngine
    static void regTypesToEngine( QQmlEngine* );


private:
    void *m_obj;
    Q_DISABLE_COPY( PermMod )
};


}

#endif
