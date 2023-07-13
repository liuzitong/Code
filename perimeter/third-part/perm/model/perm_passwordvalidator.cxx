#include "perm_passwordvalidator.hxx"
#include "perm/common/perm_memcntr.hxx"

namespace FcPerm {

#define T_PrivPtr( o )  perm_objcast( PasswordValidatorPriv*, o )
class PERM_HIDDEN PasswordValidatorPriv {
private:
    PasswordValidator  *m_parent;
    int  m_min_size, m_max_size;

public :
    PasswordValidatorPriv ( PasswordValidator *pa );
    ~PasswordValidatorPriv (  ) { }

    inline int & minSize( ) { return m_min_size; }
    inline int & maxSize( ) { return m_max_size; }
    void  fixup( QString & );
    QValidator::State  validate( QString &,int& );
};
// =====================================================================
// CTOR
// =====================================================================
PasswordValidatorPriv :: PasswordValidatorPriv ( PasswordValidator *pa )
{
    m_parent = pa;
    m_min_size = 6;
    m_max_size = 20;
}

// =====================================================================
// fixup the string
// =====================================================================
void  PasswordValidatorPriv :: fixup ( QString &str )
{
    int pos = 1; this->validate( str, pos );
}

// ======================================================================
// validate the string
// ======================================================================
QValidator::State  PasswordValidatorPriv :: validate( QString &input, int &pos )
{
    if ( input.length() <= m_min_size )
    {
        input = QStringLiteral("000000").replace(0, pos, input.left(pos));
        return PasswordValidator::Acceptable;
    }

    if ( input.length() > m_max_size )
    {
        input = input.left(m_max_size);
        return PasswordValidator::Acceptable;
    }

    if ( pos <= m_min_size )
    {
        input = QStringLiteral("000000").replace(0, pos, input.left(pos));
        return PasswordValidator::Acceptable;
    }

    return PasswordValidator::Intermediate;
}

// /////////////////////////////////////////////////////////////////////
//
//    PasswordValidator
//
// /////////////////////////////////////////////////////////////////////
// =====================================================================
//!  CTOR
// =====================================================================
PasswordValidator :: PasswordValidator ( QObject *pa ) : QValidator( pa )
{
    m_obj = perm_new( PasswordValidatorPriv, this );
}

// =====================================================================
//!  DTOR
// =====================================================================
PasswordValidator :: ~PasswordValidator (  )
{
    perm_delete( m_obj, PasswordValidatorPriv );
}

// =====================================================================
//! return minimize year
// =====================================================================
int  PasswordValidator :: minSize_ts( ) const
{ return T_PrivPtr( m_obj )->minSize(); }

// =====================================================================
//! return maximum year
// =====================================================================
int  PasswordValidator :: maxSize_ts( ) const
{ return T_PrivPtr( m_obj )->maxSize(); }

// =====================================================================
//! set the minimum year
// =====================================================================
void PasswordValidator :: setMinSize_ts( int s )
{ T_PrivPtr( m_obj )->minSize() = s; }

// =====================================================================
//! set the maximum year
// =====================================================================
void  PasswordValidator :: setMaxSize_ts( int s )
{ T_PrivPtr( m_obj )->maxSize() = s; }

// =====================================================================
//! fixup the input string
// =====================================================================
void  PasswordValidator :: fixup   ( QString &input ) const
{ T_PrivPtr( m_obj )->fixup( input ); }

// =====================================================================
//! validate the input string
// =====================================================================
PasswordValidator::State  PasswordValidator :: validate( QString &input, int &pos ) const
{   return T_PrivPtr( m_obj )->validate( input, pos ); }

}
