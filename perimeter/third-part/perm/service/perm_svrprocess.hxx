#ifndef PERM_SVRPROCESS_HXX
#define PERM_SVRPROCESS_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace FcPerm {

///////////////////////////////////////////////////
///
/// SvrProcess
///
//////////////////////////////////////////////////
class PERM_API SvrProcess : public QxPack::IcSvcBase
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SvrProcess( );

    virtual ~SvrProcess( ) Q_DECL_OVERRIDE;

    bool  isWorking( ) const;
    void  start( const QString &program, const QStringList &arguments );

    Q_SIGNAL  void  started( );
    Q_SIGNAL  void  finished( );

private:
    void *m_obj;
    Q_DISABLE_COPY( SvrProcess )
};

}

#endif // PERM_SVRPROCESS_HXX
