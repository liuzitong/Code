#ifndef QXPACK_IC_ALGOR_BYTEORDER_HPP
#define QXPACK_IC_ALGOR_BYTEORDER_HPP

#include <type_traits>
#include <utility>
#include <functional>
#include <cstdint>

#ifndef QXPACK_IC_CXX11
#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#  define QXPACK_IC_CXX11
#else
#  error "this template require C++11 !"
#endif
#endif

// ////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// : API list
// ----------------------------------------------------------------------------
// hostByteOrder -- check the host byte order
// (* if user want to reverse byte order, can use revOrderAry() in qxpack_ic_algor_ary.hpp )
//
// for avoid address alignment, below APIs are designed by reading/writing in bytes.
//
// readData16_LE -- read 16bit data from raw byte array, order is little endian
// readData32_LE -- read 32bit data from raw byte array, order is little endian
// readData64_LE -- read 64bit data from raw byte array, order is little endian
// readData16_BE -- big endian order for reading 16bit
// readData32_BE -- big endian order for reading 32bit
// readData64_BE -- big endian order for reading 64bit
//
// readData16_IsBE -- readData16_LE/BE auto select by param 'is_be'
// readData32_IsBE -- like 'readData16_IsBE'
// readData64_IsBE -- like 'readData16_IsBE'
//
// writeData16_LE -- write 16bit data to array, order is LE
// writeData32_LE -- write 32bit data to array, order is LE
// writeData64_LE -- write 64bit data to array, order is LE
// writeData16_BE -- write 16bit data to array, order is BE
// writeData32_BE -- write 32bit data to array, order is BE
// writeData64_BE -- write 64bit data to array, order is BE
//
// writeData16_IsBE -- writeData16_LE/BE auto selected by param 'is_be'
// writeData32_IsBE -- like 'writeData16_IsBE'
// writeData64_IsBE -- like 'writeData16_IsBE'
//
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {
namespace IcAlgor {

// ============================================================================
// check if byteorder is BE or LE
// ============================================================================
enum  ByteOrder {
    ByteOrder_LittleEndian = 0, ByteOrder_BigEndian
};

template <typename T = short >
inline auto  hostByteOrder( const T& v = 0 ) -> ByteOrder
{
    (void)(v);
    static_assert ( std::is_arithmetic<T>::value, "T must be arithmetic type!" );
    union { T n; int16_t n2; uint8_t n3; } uv; uv.n2 = 0x0102;
    return ( uv.n3 == 0x02 ? ByteOrder_LittleEndian :  ByteOrder_BigEndian );
}

// ============================================================================
// read the data by spec. byte order
// ============================================================================
template <typename T = int16_t >
inline auto  readData16_LE ( const void *dp ) -> T
{
    static_assert( std::is_arithmetic<T>::value && sizeof(T) == 2, "T must be 2 bytes arithmetic type! e.g int16_t or uint16_t." );
    auto ptr = reinterpret_cast<const uint8_t*>( dp );
    return  T( ptr[0] | ( uint16_t( ptr[1] ) << 8 ));
}

template <typename T = int16_t >
inline auto  readData16_BE( const void *dp ) -> T
{
    static_assert( std::is_arithmetic<T>::value && sizeof(T) == 2, "T must be 2 bytes arithmetic type! e.g int16_t or uint16_t." );
    auto ptr = reinterpret_cast<const uint8_t*>( dp );
    return  T( ( uint16_t( ptr[0] ) << 8 ) | ptr[1] );
}

template <typename T = int16_t >
inline auto  readData16_IsBE( const void *dp, bool is_be ) -> T 
{   return ( ! is_be ? readData16_LE<T>( dp ) : readData16_BE<T>( dp )); }

template <typename T = int32_t >
inline auto  readData32_LE ( const void *dp ) -> T
{
    static_assert( std::is_arithmetic<T>::value && sizeof(T) == 4, "T must be 4 bytes arithmetic type! e.g int32_t or uint32_t." );
    auto ptr = reinterpret_cast<const uint8_t*>( dp );
    uint32_t rsl = uint32_t( ptr[0] ) | ( uint32_t( ptr[1]) << 8 ) | ( uint32_t( ptr[2]) << 16 ) | ( uint32_t( ptr[3]) << 24 );
    return *( reinterpret_cast<T*>( &rsl ));
}

template <typename T = int32_t >
inline auto  readData32_BE ( const void *dp ) -> T
{
    static_assert( std::is_arithmetic<T>::value && sizeof(T) == 4, "T must be 4 bytes arithmetic type! e.g int32_t or uint32_t." );
    auto ptr = reinterpret_cast<const uint8_t*>( dp );
    uint32_t rsl = ( uint32_t( ptr[0]) << 24 ) | ( uint32_t( ptr[1]) << 16 ) | ( uint32_t( ptr[2]) << 8 ) | uint32_t( ptr[3] );
    return *( reinterpret_cast<T*>( &rsl ));
}

template <typename T = int32_t >
inline auto  readData32_IsBE( const void *dp, bool is_be ) -> T
{   return ( ! is_be ? readData32_LE<T>( dp ) : readData32_BE<T>( dp )); }


template <typename T = int64_t >
inline auto  readData64_LE ( const void *dp ) -> T
{
    static_assert( std::is_arithmetic<T>::value && sizeof(T) == 8, "T must be 8 bytes arithmetic type! e.g int64_t or uint64_t." );
    auto ptr = reinterpret_cast<const uint8_t*>( dp );
    uint32_t rsl0 = uint32_t( ptr[0] ) | ( uint32_t( ptr[1]) << 8 ) | ( uint32_t( ptr[2]) << 16 ) | ( uint32_t( ptr[3]) << 24 );
    uint32_t rsl1 = uint32_t( ptr[4] ) | ( uint32_t( ptr[5]) << 8 ) | ( uint32_t( ptr[6]) << 16 ) | ( uint32_t( ptr[7]) << 24 );
    auto rsl = ( uint64_t( rsl0 ) | ( uint64_t( rsl1 ) << 32 ));
    return *( reinterpret_cast<T*>( &rsl ));
}

template <typename T = int64_t >
inline auto  readData64_BE ( const void *dp ) -> T
{
    static_assert( std::is_arithmetic<T>::value && sizeof(T) == 8, "T must be 8 bytes arithmetic type! e.g int64_t or uint64_t." );
    auto ptr = reinterpret_cast<const uint8_t*>( dp );
    uint32_t rsl0 = ( uint32_t( ptr[0]) << 24 ) |  ( uint32_t( ptr[1]) << 16 ) | ( uint32_t( ptr[2]) << 8 ) | uint32_t( ptr[3] );
    uint32_t rsl1 = ( uint32_t( ptr[4]) << 24 ) |  ( uint32_t( ptr[5]) << 16 ) | ( uint32_t( ptr[6]) << 8 ) | uint32_t( ptr[7] );
    auto rsl = ( ( uint64_t( rsl0 ) << 32 ) | uint64_t( rsl1 ) );
    return *( reinterpret_cast<T*>( &rsl ));
}

template <typename T = int64_t >
inline auto  readData64_IsBE( const void *dp, bool is_be ) -> T
{   return ( ! is_be ? readData64_LE<T>( dp ) : readData64_BE<T>( dp )); }



// ============================================================================
// write the data by spec. order
// ============================================================================
template <typename T >
inline auto  writeData16_LE ( void *dp, const T &v ) -> void
{ (void)(dp); (const void)(v); std::abort(); }

template <typename T>
inline auto  writeData16_BE ( void *dp, const T &v ) -> void
{ (void)(dp); (const void)(v); std::abort(); }

template <>
inline auto  writeData16_LE ( void *dp, const int16_t &v ) -> void
{
   auto ptr = reinterpret_cast<uint8_t*>(dp);
   ptr[0] = uint8_t( v ); ptr[1] = uint8_t(( v >> 8 ));
}

template <>
inline auto  writeData16_BE ( void *dp, const int16_t &v ) -> void
{
   auto ptr = reinterpret_cast<uint8_t*>(dp);
   ptr[0] = uint8_t(( v >> 8 )); ptr[1] = uint8_t( v );
}

template <>
inline auto  writeData16_LE ( void *dp, const uint16_t &v ) -> void
{  writeData16_LE( dp, * reinterpret_cast<const int16_t*>(&v) ); }

template <>
inline auto  writeData16_BE ( void *dp, const uint16_t &v ) -> void
{  writeData16_BE( dp, * reinterpret_cast<const int16_t*>(&v)); }

template <typename T>
inline auto  writeData16_IsBE( void *dp, const T&v, bool is_be ) -> void
{ return ( ! is_be ? writeData16_LE( dp, v ) : writeData16_BE( dp, v )); }

template <typename T>
inline auto  writeData32_LE ( void *dp, const T &v ) -> void
{ (void)(dp);(const void)(v); std::abort(); }

template <typename T>
inline auto  writeData32_BE ( void *dp, const T &v ) -> void
{ (void)(dp);(const void)(v); std::abort(); }

template <>
inline auto  writeData32_LE ( void *dp, const int32_t &v ) -> void
{
    auto ptr = reinterpret_cast<uint8_t*>( dp );
    ptr[0] = uint8_t(( v      ));
    ptr[1] = uint8_t(( v >> 8 ));
    ptr[2] = uint8_t(( v >>16 ));
    ptr[3] = uint8_t(( v >>24 ));
}

template <>
inline auto  writeData32_BE ( void *dp, const int32_t &v ) -> void
{
    auto ptr = reinterpret_cast<uint8_t*>( dp );
    ptr[0] = uint8_t(( v >>24 ));
    ptr[1] = uint8_t(( v >>16 ));
    ptr[2] = uint8_t(( v >> 8 ));
    ptr[3] = uint8_t(( v      ));
}

template <>
inline auto  writeData32_LE ( void *dp, const uint32_t &v ) -> void
{ writeData32_LE( dp, * reinterpret_cast<const int32_t*>(&v)); }

template <>
inline auto  writeData32_BE ( void *dp, const uint32_t &v ) -> void
{ writeData32_BE( dp, * reinterpret_cast<const int32_t*>(&v)); }

template <>
inline auto  writeData32_LE ( void *dp, const float &v ) -> void
{
    static_assert( sizeof(float) == 4, "float must be 4 bytes." );
    auto ptr = reinterpret_cast<const void*>(&v);
    writeData32_LE( dp, * reinterpret_cast<const int32_t*>(ptr) );
}

template <>
inline auto  writeData32_BE ( void *dp, const float &v ) -> void
{
    static_assert( sizeof(float) == 4, "float must be 4 bytes." );
    auto ptr = reinterpret_cast<const void*>(&v);
    writeData32_BE( dp, * reinterpret_cast<const int32_t*>(ptr) );
}

template <typename T>
inline auto  writeData32_IsBE( void *dp, const T&v, bool is_be ) -> void
{ return ( ! is_be ? writeData32_LE( dp, v ) : writeData32_BE( dp, v )); }


template <typename T>
inline auto  writeData64_LE ( void *dp, const T &v ) -> void
{ (void)(dp); (const void)(v); std::abort(); }

template <typename T>
inline auto  writeData64_BE ( void *dp, const T &v ) -> void
{ (void)(dp); (const void)(v); std::abort(); }

template <>
inline auto  writeData64_LE ( void *dp, const int64_t &v ) -> void
{
    auto ptr = reinterpret_cast<uint8_t*>( dp );
    auto lo_part = uint32_t(( v       ));
    auto hi_part = uint32_t(( v >> 32 ));

    ptr[0] = uint8_t(( lo_part      ));
    ptr[1] = uint8_t(( lo_part >> 8 ));
    ptr[2] = uint8_t(( lo_part >>16 ));
    ptr[3] = uint8_t(( lo_part >>24 ));
    ptr[4] = uint8_t(( hi_part      ));
    ptr[5] = uint8_t(( hi_part >> 8 ));
    ptr[6] = uint8_t(( hi_part >>16 ));
    ptr[7] = uint8_t(( hi_part >>24 ));
}

template <>
inline auto  writeData64_BE ( void *dp, const int64_t &v ) -> void
{
    auto ptr = reinterpret_cast<uint8_t*>( dp );
    auto lo_part = uint32_t(( v       ));
    auto hi_part = uint32_t(( v >> 32 ));

    ptr[0] = uint8_t(( hi_part >>24 ));
    ptr[1] = uint8_t(( hi_part >>16 ));
    ptr[2] = uint8_t(( hi_part >> 8 ));
    ptr[3] = uint8_t(( hi_part      ));
    ptr[4] = uint8_t(( lo_part >>24 ));
    ptr[5] = uint8_t(( lo_part >>16 ));
    ptr[6] = uint8_t(( lo_part >> 8 ));
    ptr[7] = uint8_t(( lo_part      ));
}

template <>
inline auto  writeData64_LE ( void *dp, const uint64_t &v ) -> void
{ return writeData64_LE( dp, * reinterpret_cast<const int64_t*>( &v )); }

template <>
inline auto  writeData64_BE ( void *dp, const uint64_t &v ) -> void
{ return writeData64_BE( dp, * reinterpret_cast<const int64_t*>( &v )); }

template <>
inline auto  writeData64_LE ( void *dp, const double &v ) -> void
{
    static_assert( sizeof(double) == 8, "double must be 8 bytes." );
    auto ptr = reinterpret_cast<const void*>(&v);
    writeData64_LE( dp, * reinterpret_cast<const int64_t*>(ptr) );
}

template <>
inline auto  writeData64_BE ( void *dp, const double &v ) -> void
{
    static_assert( sizeof(double) == 8, "double must be 8 bytes." );
    auto ptr = reinterpret_cast<const void*>(&v);
    writeData64_BE( dp, * reinterpret_cast<const int64_t*>(ptr) );
}

template <typename T>
inline auto  writeData64_IsBE( void *dp, const T&v, bool is_be ) -> void
{ return ( ! is_be ? writeData64_LE( dp, v ) : writeData64_BE( dp, v )); }


}
}
#endif
