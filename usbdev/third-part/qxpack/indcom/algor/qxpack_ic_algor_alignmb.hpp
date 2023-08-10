#ifndef QXPACK_IC_ALGOR_ALIGNEDMEMBLOCK_HPP
#define QXPACK_IC_ALGOR_ALIGNEDMEMBLOCK_HPP

#include <cstdint>
#include <cstdlib>

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
// AlignedMemBlock -- a class used to return aligned address pointer
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {
namespace IcAlgor {

class  AlignedMemBlock {
private:
    uint8_t *m_ptr; size_t m_pad;
private:
    AlignedMemBlock( const AlignedMemBlock& );
    AlignedMemBlock & operator = ( const AlignedMemBlock & );
public :
    explicit AlignedMemBlock( size_t N, size_t req_sz )
    {
        auto n = ( N > 0 ? N : 1 );
        auto total_sz = req_sz + ( n > 1 ? n : 0 ); // except 0 and 1, other align value maybe need append size...
        m_ptr = reinterpret_cast<uint8_t*>( std::malloc( total_sz ) );
        if ( n > 1 ) {
            auto ptr_val = reinterpret_cast<uintptr_t>( m_ptr );
            m_pad = ( N - ( ptr_val % n )) % N;
        } else {
            m_pad = 0;
        }
    }

    ~AlignedMemBlock( )
    {
        if ( m_ptr != nullptr ) {
            std::free( m_ptr ); m_ptr = nullptr;
        }
    }
    inline auto rawPtr() -> void* { return m_ptr; }
    inline auto padding()-> size_t{ return m_pad; }
    inline auto dataPtr()-> void* { return reinterpret_cast<void*>( m_ptr + m_pad ); }
};



}
}
#endif
