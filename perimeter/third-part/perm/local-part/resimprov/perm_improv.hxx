#ifndef  PERM_IMPROV_HXX
#define  PERM_IMPROV_HXX

#include <QQuickImageProvider>
#include <QImage>
#include "perm/common/perm_def.h"

namespace FcPerm {

// /////////////////////////////////////////////////////////////////////
//! PermImProv
/*!
  implement the a image provider. this class is used for provide all image for \n
Perm module
*/
// ///////////////////////////////////////////////////////////////////// 
class  PERM_HIDDEN  PermImProv : public QQuickImageProvider {

public:
    // =================================================================
    // Functions
    // =================================================================
    PermImProv (  );
    virtual ~PermImProv( ) Q_DECL_OVERRIDE;

    virtual QImage requestImage( const QString &id, QSize *size, const QSize &reqSize ) Q_DECL_OVERRIDE;
    
private:
    // =================================================================
    // var.
    // =================================================================
    void *m_obj;
    Q_DISABLE_COPY( PermImProv )

};
    


}

/*!
  @}
*/

/*!
  @}
*/

/*!
  @}
*/

#endif
