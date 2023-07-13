#ifndef PERM_PASSWORDVALIDATOR_HXX
#define PERM_PASSWORDVALIDATOR_HXX

#include "perm/common/perm_def.h"
#include <QValidator>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
//
// 密码输入框校验器
//
// ////////////////////////////////////////////////////////////////////////////
class PERM_API PasswordValidator : public QValidator  {
    Q_OBJECT

    Q_PROPERTY( int  minSize  READ  minSize_ts  WRITE setMinSize_ts  NOTIFY minSizeChanged )
    Q_PROPERTY( int  maxSize  READ  maxSize_ts  WRITE setMaxSize_ts  NOTIFY maxSizeChanged )

public:
    // =================================================================
    // Funtions
    // =================================================================
    PasswordValidator( QObject *pa = Q_NULLPTR );
    virtual ~PasswordValidator(  ) Q_DECL_OVERRIDE;

    int   minSize_ts( ) const;
    int   maxSize_ts( ) const;

    void  setMinSize_ts( int );
    void  setMaxSize_ts( int );

    virtual void  fixup   ( QString & ) const Q_DECL_OVERRIDE;
    virtual State validate( QString &, int & ) const Q_DECL_OVERRIDE;

    Q_SIGNAL void minSizeChanged( );
    Q_SIGNAL void maxSizeChanged( );

private:
    void *m_obj;
    Q_DISABLE_COPY( PasswordValidator )
};

}

#endif // PERM_PASSWORDVALIDATOR_HXX
