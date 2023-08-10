#ifndef QXPACK_IC_ALGOR_ERRDEVT_HPP
#define QXPACK_IC_ALGOR_ERRDEV_HPP

#include <cstdlib>
#include <cmath>
#include <cstdint>

#include "qxpack_ic_algor_base.hpp"

#ifndef QXPACK_IC_CXX11
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#  define QXPACK_IC_CXX11
#else
#  error "this template require C++11 !"
#endif
#endif

// ////////////////////////////////////////////////////////////////////////////
// : API list
// ----------------------------------------------------------------------------
// isErrDevLess    -- check if error dev. is less than spec. eps.
// isErrDevGreater -- check if error dev. is greater than spec. eps.
// isErrDevLessEqu -- check if error dev. is less or equal than spec. eps.
// isErrDevGreaterEqu -- check if error dev. is greater or equal than spec. eps
//
// ////////////////////////////////////////////////////////////////////////////

namespace QxPack {
namespace IcAlgor {
// ============================================================================
// abs(p1 - p2) < asb(eps)
// ============================================================================
template <typename T>
inline auto  isErrDevLess( const T&p1, const T&p2, const T&eps ) -> bool
{  return ( abs(p1-p2)) < abs(eps);  }

template <>
inline auto  isErrDevLess( const double &p1, const double &p2, const double &eps ) -> bool
{  return int64_t(( abs(p1-p2) * 1000000000000. )) < int64_t( eps * 1000000000000. ); }

template <>
inline auto  isErrDevLess( const float &p1, const float &p2, const float &eps) -> bool
{  return int64_t(( abs(p1-p2) * 100000.f )) < int64_t( eps * 100000.f ); }

// ============================================================================
// abs(p1-p2) > abs(eps)
// ============================================================================
template <typename T>
inline auto  isErrDevGreater( const T&p1, const T&p2, const T&eps ) -> bool
{  return ( abs(p1-p2)) > abs(eps);  }

template <>
inline auto  isErrDevGreater( const double &p1, const double &p2, const double &eps) -> bool
{  return int64_t(( abs(p1-p2) * 1000000000000. )) > int64_t( eps * 1000000000000. ); }

template <>
inline auto  isErrDevGreater( const float &p1, const float &p2, const float &eps ) -> bool
{  return int64_t(( abs(p1-p2) * 100000.f )) > int64_t( eps * 100000.f ); }

// ============================================================================
// ! ( abs(p1,p2) > abs(eps) )
// ============================================================================
template <typename T>
inline auto  isErrDevLessEqu( const T&p1, const T&p2, const T&eps ) -> bool
{  return ! isErrDevGreater( p1,p2,eps); }

// ============================================================================
// ! ( abs(p1,p2) < abs( eps ))
// ============================================================================
template <typename T>
inline auto  isErrDevGreaterEqu(  const T&p1, const T&p2, const T&eps ) -> bool
{  return ! isErrDevLess( p1, p2, eps ); }


// ============================================================================
// array version compare
// ============================================================================
namespace ErrDevHelper {
struct CmpTypeLess       {};
struct CmpTypeLessEqu    {};
struct CmpTypeGreater    {};
struct CmpTypeGreaterEqu {};

template <typename T, typename E>
inline auto  _isErrDevXX( const E&, const T&, const T&, const T& ) ->bool
{ return false; }

template <typename T>
inline auto  _isErrDevXX( const ErrDevHelper::CmpTypeLess&, const T&p1, const T&p2, const T&eps ) -> bool
{ return isErrDevLess( p1, p2, eps ); }

template <typename T>
inline auto  _isErrDevXX( const ErrDevHelper::CmpTypeLessEqu&, const T&p1, const T&p2, const T&eps) -> bool
{ return isErrDevLessEqu( p1, p2, eps ); }

template <typename T>
inline auto  _isErrDevXX( const ErrDevHelper::CmpTypeGreater&, const T&p1, const T&p2, const T&eps) -> bool
{ return isErrDevGreater( p1, p2, eps ); }

template <typename T>
inline auto  _isErrDevXX( const ErrDevHelper::CmpTypeGreaterEqu&, const T&p1, const T&p2, const T&eps) -> bool
{ return isErrDevGreaterEqu( p1, p2, eps ); }

template <typename T, typename E>
auto     isErrDevXX( const E &e, const T *p1, const T *p2, const T &eps, size_t n ) -> bool
{
    auto ret = true; size_t i = 0;
    while ( ret && i < n ) { ret = _isErrDevXX( e, p1[i], p2[i], eps ); ++ i; }
    return ret;
}
}

template <typename T>
inline auto  isErrDevLess( const T *p1, const T *p2, const T &eps, size_t n ) -> bool
{  return ErrDevHelper::isErrDevXX( ErrDevHelper::CmpTypeLess(), p1, p2, eps, n ); }

template <typename T>
inline auto  isErrDevGreater( const T *p1, const T *p2, const T &eps, size_t n ) -> bool
{  return ErrDevHelper::isErrDevXX( ErrDevHelper::CmpTypeGreater(), p1, p2, eps, n ); }

template <typename T>
inline auto  isErrDevLessEqu( const T *p1, const T *p2, const T &eps, size_t n ) -> bool
{ return  ErrDevHelper::isErrDevXX( ErrDevHelper::CmpTypeLessEqu(), p1, p2, eps, n ); }

template <typename T>
inline auto  isErrDevGreaterEqu( const T *p1, const T *p2, const T &eps, size_t n ) -> bool
{ return  ErrDevHelper::isErrDevXX( ErrDevHelper::CmpTypeGreaterEqu(), p1, p2, eps, n ); }


}
}
#endif
