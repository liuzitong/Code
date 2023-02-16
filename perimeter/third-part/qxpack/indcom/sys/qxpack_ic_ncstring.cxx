// ////////////////////////////////////////////////////////////////////////////
// @author nightwing
// @history
//        2018/12/20  modified, for GCC 5 compiler
//        2019/04/27  checked.
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_NCSTRING_CXX
#define QXPACK_IC_NCSTRING_CXX

#include "qxpack_ic_ncstring.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// IcNcString class
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcNcString :: IcNcString ( ) { m_obj = nullptr; }

IcNcString :: IcNcString ( const char *str )
{  m_str = str; }

IcNcString :: IcNcString ( const QString &str )
{  m_str = str; }

IcNcString :: IcNcString ( const QByteArray &ba )
{  m_str = ba;  }

IcNcString :: IcNcString ( const IcNcString &str )
{  m_str = str.m_str; }

// ============================================================================
// dtor
// ============================================================================
IcNcString :: ~IcNcString ( ) { }

// ============================================================================
// operator <
// ============================================================================
bool  IcNcString :: operator < ( const IcNcString &other ) const
{
    return !( m_str.compare( other.m_str, Qt::CaseInsensitive ) >= 0 );
}

// ============================================================================
// setup string
// ============================================================================
void   IcNcString :: setString( const QString &str )
{
    m_str = str;
}

// ============================================================================
// string ref.
// ============================================================================
const QString &   IcNcString::strRef( ) const
{   return m_str; }


}

#endif
