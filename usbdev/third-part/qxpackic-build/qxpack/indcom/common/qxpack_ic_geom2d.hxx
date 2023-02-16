/* ///////////////////////////////////////////////////////////////////////// */
/*!
  @file    IcGeom2d
  @author  night wing
  @date    2020/07
  @brief   The file implement the geometry 2D object's
  @par     History
  @verbatim
          <author>   <time>   <version>  <desc>
         nightwing   2020/07   0.1.0     build this module
  @endverbatim
*/
/* ///////////////////////////////////////////////////////////////////////// */
#ifndef QXPACK_IC_GEOM2D_HXX
#define QXPACK_IC_GEOM2D_HXX

#include "qxpack_ic_global.hxx"
#include "qxpack_ic_geom2dmath.hxx"
#include <cmath>

namespace QxPack {

template <typename T> class IcPoint2D;
template <typename T> class IcVector2D;
template <typename T> class IcLine2D;

// ////////////////////////////////////////////////////////////////////////////
//
//                      vector 2d
//
// ////////////////////////////////////////////////////////////////////////////
template <typename T >
class IcVector2D {
private: T m_x, m_y;  // order is same as Qt5::QVector2D ( T is float )
public :
    //! @brief ctor by default, x and y is 0
    explicit IcVector2D( ) : m_x(0), m_y(0)
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief dtor
    ~IcVector2D() { }

    //! @brief ctor by other
    IcVector2D( const IcVector2D<T> &other ) : m_x( other.m_x ), m_y( other.m_y )
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief ctor by type
    //! @note  type must be floating or integeral
    IcVector2D( const T &x, const T&y ) : m_x(x), m_y(y)
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief ctor by point
    //! @note vector AB means Point B - Point A
    IcVector2D( const IcPoint2D<T> &a, const IcPoint2D<T> &b ) : m_x( b.x() - a.x() ), m_y( b.y() - a.y())
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief assign copy
    IcVector2D<T> &  operator = ( const IcVector2D<T> &other ) { m_x = other.m_x, m_y = other.m_y; return *this; }

#ifdef QXPACK_IC_CXX11
    //! @brief C++11's move ctor
    IcVector2D( IcVector2D<T> && other )
    { m_x = other.m_x; m_y = other.m_y; }

    //! @brief c++11's move assign
    IcVector2D<T> & operator = ( IcVector2D<T> && other )
    { m_x = other.m_x; m_y = other.m_y; return *this; }
#endif

    //! @brief check if current vector is zero vector ( x == y == 0 )
    inline bool isNull() const { return fuzzyIsNull( m_x ) && fuzzyIsNull( m_y ); }
    inline T    x()    const   { return m_x; }
    inline T    y()    const   { return m_y; }
    inline T&   xRef()         { return m_x; }
    inline T&   yRef()         { return m_y; }
    inline T&   rx()           { return m_x; }
    inline T&   ry()           { return m_y; }

    //! @return the norm type 1 value
    inline double   norm1() const { return abs( m_x ) + abs( m_y ); }

    //! @return the norm type 2 value
    inline double   norm2() const
    {
        double dx = abs( m_x ); double dy = abs( m_y );
        if ( dx > dy ) { dy /= dx; return dx * std::sqrt( 1.0 + dy * dy ); }
        if ( dy > 0  ) { dx /= dy; return dy * std::sqrt( 1.0 + dx * dx ); }
        return 0;
    }

    //! @brief normalize this vector
    inline IcVector2D<T> &  normalize()
    {
        if ( ! isNull() ) {
            double m = norm2();
            if ( !fuzzyIsNull( m ) ) { m_x /= m; m_y /= m; }
            else { m_x = ( m_x >= 0 ? DBL_MAX : -DBL_MAX ); m_y = ( m_y >= 0 ? DBL_MAX : -DBL_MAX ); }
        }
        return *this;
    }

    //! @brief construct a new vector by this, and normalize it
    inline IcVector2D<T>    normalized() const { return ( IcVector2D<T>( *this )).normalize(); }

    //! @brief scale this vector by factor
    //! @note  fact > 0 means growth the vector length; fact < 0 means flip the vector and growth it
    inline IcVector2D<T> &  scale( const T &fact )
    {  m_x *= fact; m_y *= fact; return *this; }

    //! @brief return a new vector that scaled
    inline IcVector2D<T>    scaled( const T &fact )
    {  return  IcVector2D<T>( *this ).scale( fact ); }

    //! @return the vertical vector of this vector
    inline IcVector2D<T>    ccwVertVector( bool req_norm ) const
    {
        IcVector2D<double> vv = IcGeom2DMath::ccwVertVector( IcVector2D<double>( double( m_x ), double( m_y )), req_norm );
        return IcVector2D<T>( T( vv.x()), T( vv.y()));
    }

    //! @return the vertical vector of this vector
    inline IcVector2D<T>    cwVertVector( bool req_norm ) const
    {   IcVector2D<T> vv = ccwVertVector( req_norm );
        vv.m_x *= -1; vv.m_y *= -1; return vv;
    }

    //! @brief static function belong to IcVector2D
    //! @return  scalar value, vector ab( b - a ) dot vector bc( c - b ).
    template <typename Tx>
    inline static double  dotProduct( const IcVector2D<Tx> &ab, const IcVector2D<Tx> &bc )
    { return double( ab.m_x * bc.m_x + ab.m_y * bc.m_y ); }

    //! @brief static fucntion belong to IcVector2D
    //! @return normal vector, vector ab( b - a ) cross vector ac( c - a )
    //! @note   right handle rule: thumb(abxac), index finger(vec ab), mid. finger( vec ac )
    template <typename Tx>
    inline static double  crossProduct( const IcVector2D<Tx> &ab, const IcVector2D<Tx> &ac )
    {   return double( ab.x() * ac.y() - ab.y() * ac.x()); }

    //! @return check if this vector is at clocked direction of spec. vector
    inline bool    isAtCWdirOf( const IcVector2D<T> &vec ) const
    {  return crossProduct( vec, *this ) < 0;  }

    //! @return check if this vector is at anti-clocked direction of spec. vector
    inline bool    isAtCCWdirOf( const IcVector2D<T> &vec ) const
    {  return crossProduct( vec, *this ) > 0;  }

    //! @return check if this vector is parallel of spec. vector
    inline bool    isParallelOf( const IcVector2D<T> &vec, int fp_num = 9 ) const
    {  return fuzzyIsNull( crossProduct( vec, *this ), fp_num ); }

    //! @return check if this vector is vertical of spec. vector
    inline bool    isVerticalOf( const IcVector2D<T> &vec, int fp_num = 9 ) const
    {  return fuzzyIsNull( dotProduct( *this, vec ), fp_num ); }

    //! @brief global operator ==
    //! @return compare two vector, check if them are same value
    template <typename Tx>
    friend inline  bool  operator==( const IcVector2D<Tx> &a, const IcVector2D<Tx> &b );

    //! @brief global operator !=
    //! @return compare two vector, check if them are not same value
    template <typename Tx>
    friend inline  bool  operator!=( const IcVector2D<Tx> &a, const IcVector2D<Tx> &b );

    //! @brief global operator +
    //! @return result of  a + b
    template <typename Tx>
    friend inline const IcVector2D<Tx>  operator+( const IcVector2D<Tx> &a, const IcVector2D<Tx> &b);

    //! @brief global operator -
    //! @return result of a - b
    template <typename Tx>
    friend inline const IcVector2D<Tx>  operator-( const IcVector2D<Tx> &a, const IcVector2D<Tx> &b );
};

typedef IcVector2D<double>  IcVector2Ddbl;

template <typename T>
  inline  bool  operator==( const IcVector2D<T> &a, const IcVector2D<T> &b )
{  return fuzzyCompare( a.x(), b.x() ) && fuzzyCompare( a.y(), b.y() ); }

template <typename T>
  inline  bool  operator!=( const IcVector2D<T> &a, const IcVector2D<T> &b )
{  return ( ! fuzzyCompare( a.x(), b.x() )) ||  ( ! fuzzyCompare( a.y(), b.y() )) ; }

template <typename T>
    inline const IcVector2D<T> operator+( const IcVector2D<T> &a, const IcVector2D<T> &b)
{ return IcVector2D<T>( a.m_x + b.m_x, a.m_y + b.m_y ); }

template <typename T>
    inline const IcVector2D<T> operator-( const IcVector2D<T> &a, const IcVector2D<T> &b)
{ return IcVector2D<T>( a.m_x - b.m_x, a.m_y - b.m_y ); }




// ////////////////////////////////////////////////////////////////////////////
//
//                      point 2d
//
// ////////////////////////////////////////////////////////////////////////////
template <typename T >
class IcPoint2D {
private: T m_x, m_y;  // order is same as Qt5::QPoint
public :
    //! @brief default ctor, point at (0,0)
    explicit IcPoint2D( ) : m_x(0), m_y(0)
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief dtor
    ~IcPoint2D() { }

    //! @brief ctor by other
    IcPoint2D( const IcPoint2D<T> &other ) : m_x( other.m_x ), m_y( other.m_y )
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief ctor by type
    //! @note type must be floating or integral
    IcPoint2D( const T &x, const T&y ) : m_x(x), m_y(y)
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief assign copy
    IcPoint2D<T> &  operator = ( const IcPoint2D<T> &other ) { m_x = other.m_x; m_y = other.m_y; return *this; }

#ifdef QXPACK_IC_CXX11
    //! @brief c++11's move copy
    IcPoint2D( IcPoint2D<T> && other )
    { m_x = other.m_x; m_y = other.m_y; }

    //! @brief c++11's move copy
    IcPoint2D<T> &  operator = ( IcPoint2D<T> &&other )
    { m_x = other.m_x; m_y = other.m_y; return *this; }
#endif

    //! @brief check if this point is at (0,0), means NULL
    inline bool isNull() const { return fuzzyIsNull( m_x ) && fuzzyIsNull( m_y ); }
    inline T  x() const { return m_x; }
    inline T  y() const { return m_y; }
    inline T& xRef()    { return m_x; }
    inline T& yRef()    { return m_y; }
    inline T& rx()      { return m_x; }
    inline T& ry()      { return m_y; }
    inline const T& xConstRef() const { return m_x; }
    inline const T& yConstRef() const { return m_y; }

    inline IcPoint2D<T>&  set( const T &x, const T &y )
    { m_x = x; m_y = y; return *this; }

    //! @brief move this point by spec. vector
    //! @note point moves in the direction of vector.
    inline IcPoint2D<T>&  move( const IcVector2D<T> &vec )
    { m_x += vec.x(); m_y += vec.y(); return *this; }

    //! @return a new point that moved by vector
    inline IcPoint2D<T>   moved( const IcVector2D<T> &vec ) const
    { return IcPoint2D<T>( *this ).move( vec ); }

    //! @note  check if point a is same as point b
    template <typename Tx>
    friend inline  bool  operator==( const IcPoint2D<Tx> &a, const IcPoint2D<Tx> &b );

    //! @return a != b
    template <typename Tx>
    friend inline  bool  operator!=( const IcPoint2D<Tx> &a, const IcPoint2D<Tx> &b );
};

typedef IcPoint2D<double>  IcPoint2Ddbl;

template <typename T>
  inline  bool  operator==( const IcPoint2D<T> &a, const IcPoint2D<T> &b )
{  return fuzzyCompare( a.x(), b.x() ) && fuzzyCompare( a.y(), b.y() ); }

template <typename T>
  inline  bool  operator!=( const IcPoint2D<T> &a, const IcPoint2D<T> &b )
{  return ( ! fuzzyCompare( a.x(), b.x() )) ||  ( ! fuzzyCompare( a.y(), b.y() )) ; }




// ////////////////////////////////////////////////////////////////////////////
// 
//                      line 2d
//
// ////////////////////////////////////////////////////////////////////////////
template <typename T>
class IcLine2D {
private: IcPoint2D<T> m_pt1, m_pt2; // order is same as Qt5::QLine
public :
    //! @brief default ctor, ctor a null line
    explicit IcLine2D()
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief ctor by two points
    IcLine2D( const IcPoint2D<T>& p1, const IcPoint2D<T>& p2 ) : m_pt1( p1 ), m_pt2( p2 )
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief dtor
    ~IcLine2D() { }

    //! @brief override, ctor by two points
    IcLine2D( const T &x1, const T &y1, const T &x2, const T &y2 ) : m_pt1( x1, y1 ), m_pt2( x2, y2 )
    { QXPACK_IC_STATIC_ASSERT( std::is_floating_point<T>::value != 0 || std::is_integral<T>::value != 0, "T must be floating type or integral !"); }

    //! @brief ctor by other
    IcLine2D ( const IcLine2D<T> &other )
    { m_pt1 = other.m_pt1; m_pt2 = other.m_pt2; }

    //! @brief assign copy
    IcLine2D<T> &  operator = ( const IcLine2D<T> &other )
    { m_pt1 = other.m_pt1; m_pt2 = other.m_pt2; return *this; }

#ifdef QXPACK_IC_CXX11
    //! @brief c++11's move copy
    IcLine2D ( IcLine2D<T> &&other )
    { m_pt1 = other.m_pt1; m_pt2 = other.m_pt2; }

    //! @brief c++11's move assign
    IcLine2D<T> &  operator = ( IcLine2D<T> &&other )
    { m_pt1 = other.m_pt1; m_pt2 = other.m_pt2; return *this; }
#endif

    //! @brief check if the line is a null line ( all points are null )
    inline bool           isNull() const { return m_pt1.isNull() && m_pt2.isNull(); }

    //! @brief check if the line is a zero length line
    inline bool           isZeroLength() const { return ( m_pt1 == m_pt2 ); }

    inline IcPoint2D<T>   p1() const { return m_pt1; }
    inline IcPoint2D<T>   p2() const { return m_pt2; }
    inline IcPoint2D<T>&  p1Ref() { return m_pt1; }
    inline IcPoint2D<T>&  p2Ref() { return m_pt2; }
    inline IcPoint2D<T>&  rp1()   { return m_pt1; }
    inline IcPoint2D<T>&  rp2()   { return m_pt2; }
    inline const IcPoint2D<T>&  p1ConstRef() const { return m_pt1; }
    inline const IcPoint2D<T>&  p2ConstRef() const { return m_pt2; }
    inline IcVector2D<T>  vector() const { return IcVector2D<T>( m_pt1, m_pt2 );  }

    inline T   x1() const { return m_pt1.x(); }
    inline T   y1() const { return m_pt1.y(); }
    inline T   x2() const { return m_pt2.x(); }
    inline T   y2() const { return m_pt2.y(); }
    inline T&  rx1()      { return m_pt1.rx(); }
    inline T&  ry1()      { return m_pt1.ry(); }
    inline T&  rx2()      { return m_pt2.rx(); }
    inline T&  ry2()      { return m_pt2.ry(); }

    //! @return the center point of this line
    inline IcPoint2D<T>   center() const
    { return IcPoint2D<T>( 0.5 * m_pt1.x() + 0.5 * m_pt2.x(), 0.5 * m_pt2.y() + 0.5 * m_pt2.y() ); }

    //! @return the line length
    inline double   length() const
    {
        double dx = m_pt2.x() - m_pt1.x(); double dy = m_pt2.y() - m_pt1.y();
        dx = abs( dx ); dy = abs( dy );
        if ( dx > dy ) { dy /= dx; return dx * std::sqrt( 1.0 + dy * dy ); }
        if ( dy > 0 )  { dx /= dy; return dy * std::sqrt( 1.0 + dx * dx ); }
        return 0;
    }

    //! @brief scale this line
           IcLine2D<T>     scaled( const double &factor ) const;

    //! @brief move the line by vector
    inline IcLine2D<T> &  move( const IcVector2D<T> &vec )
    {  m_pt1.move( vec ); m_pt2.move( vec ); return *this; }

    //! @return a new line that moved by vector
    inline IcLine2D<T>    moved( const IcVector2D<T> &vec ) const
    {  return IcLine2D<T>( *this ).move( vec ); }   

    //! @return check if this line is parallel of another
    inline bool    isParallelOf( const IcLine2D<T> &ln, int fp_num = 9 ) const
    { return vector().isParallelOf( ln.vector(), fp_num ); }

    //! @return check if this line is at clockwise direction of spec. line
    inline bool    isAtCWdirOf( const IcLine2D<T> &ln  ) const
    { return vector().isAtCWdirOf( ln.vector() );  }

    //! @return check if this line is at counter clockwise direction of spec. line
    inline bool    isAtCCWdirOf( const IcLine2D<T> &ln ) const
    { return vector().isAtCCWdirOf( ln.vector() ); }

    //! @return check if this line is vertical of spec. line
    inline bool    isVerticalOf( const IcLine2D<T> &ln, int fp_num = 9 ) const
    { return vector().isVerticalOf( ln.vector(), fp_num ); }

    //! @brief this is a override function, see IcGeom2DMath::findCrossPoint()
    template <typename Tx>
    inline static int  findCrossPoint( IcPoint2D<Tx> &cpt, const IcLine2D<Tx> &ab, const IcLine2D<Tx> &bc )
    {
        IcPoint2Dfp pt; int ret = IcGeom2DMath::findCrossPoint( pt, IcLine2Dfp( ab.p1(), ab.p2()), IcLine2Dfp( bc.p1(), bc.p2()));
        cpt = IcPoint2D<Tx>( Tx( pt.x()), Tx( pt.y()));
        return ret;
    }

    //! @brief global operator ==
    template <typename Tx>
    friend inline  bool  operator==( const IcLine2D<Tx> &a, const IcLine2D<Tx> &b );

    //! @brief global operator !=
    template <typename Tx>
    friend inline  bool  operator!=( const IcLine2D<Tx> &a, const IcLine2D<Tx> &b );
};

typedef IcLine2D<double> IcLine2Ddbl;

template <typename T>
  inline  bool  operator==( const IcLine2D<T> &a, const IcLine2D<T> &b )
{  return ( a.p1() == b.p1() && a.p2() == b.p2() ) || ( a.p1() == b.p2() && a.p2() == b.p1()); }

template <typename T>
  inline  bool  operator!=( const IcLine2D<T> &a, const IcLine2D<T> &b )
{  return !( a == b ); }


template <typename T>
IcLine2D<T>   IcLine2D<T>::scaled( const double &fact ) const
{
    if ( m_pt1 == m_pt2 || fuzzyCompare( fact, 1.0 )) { return *this; }
    IcVector2D<T> vec( m_pt1, m_pt2 ), inv_vec( m_pt2, m_pt1 );
    double f2 = 0.5 * fact + 0.5;
    vec.scale( f2 ); inv_vec.scale( f2 );
    return IcLine2D<T>( m_pt2.x() + inv_vec.x(), m_pt2.y() + inv_vec.y(), m_pt1.x() + vec.x(), m_pt1.y() + vec.y() );
}


// ////////////////////////////////////////////////////////////////////////////
// mixed API
// ////////////////////////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////
// float point  point/line/vector type ( not the IcPoint2Dflt ... )
// ////////////////////////////////////////////////////////////////////////////
#if !defined( QXPACK_IC_GEOM2D_FP_DEFINED )
#define QXPACK_IC_GEOM2D_FP_DEFINED 1
typedef IcPoint2D<qxreal_t>   IcPoint2Dfp;
typedef IcVector2D<qxreal_t>  IcVector2Dfp;
typedef IcLine2D<qxreal_t>    IcLine2Dfp;
#endif

}

#endif
