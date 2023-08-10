#ifndef QXPACK_SC_MATRIX_HPP
#define QXPACK_SC_MATRIX_HPP

#include "qxpack_sc_memblock.hxx"

// ////////////////////////////////////////////////////////////////////////////
//
//                      matrix template
//
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {

template <typename T>
class QXPACK_SC_HIDDEN ScMatrix QXPACK_SC_FINAL {
private:
    ScMemBlock  m_mem;
protected:
    struct InfoHdr QXPACK_SC_FINAL { size_t hdr_sz, fix_step, dim[2]; };
    inline InfoHdr*  infoHdr() const { return  reinterpret_cast<InfoHdr*>( const_cast<void*>(m_mem.constData()) ); }
    inline T*     dataPtr() const    { InfoHdr *hdr = infoHdr(); return reinterpret_cast<T*>( reinterpret_cast<uint8_t*>(hdr) + hdr->hdr_sz ); }
public :
    // ========================================================================
    //  the element visitor used to access the element of matrix
    // ========================================================================
    class QXPACK_SC_HIDDEN ElemVisitor {
    private: size_t dim; T *ptr;
    public :
        explicit ElemVisitor( size_t d, T *p ) : dim(d), ptr(p) {}
        inline const T* dataPtr() const { return ptr; }

        inline const T& operator[]( const size_t &i ) const
        { QXPACK_SC_ASSERT( i < dim, "( i = %u, dim = %u ) out of array index!", i, dim ); return ptr[i]; }

        inline T&  operator[]( const size_t &i )
        { QXPACK_SC_ASSERT( i < dim, "( i = %u, dim = %u ) out of array index!", i, dim ); return ptr[i]; }

        inline const T& operator[]( const int &i ) const
        { QXPACK_SC_ASSERT( size_t(i) < dim, "( i = %u, dim = %u ) out of array index!", i, dim ); return ptr[i]; }

        inline T&  operator[]( const int &i )
        { QXPACK_SC_ASSERT( size_t(i) < dim, "( i = %u, dim = %u ) out of array index!", i, dim ); return ptr[i]; }
    };


    explicit ScMatrix( ){ }
    ~ScMatrix( ) { }

    ScMatrix( size_t dim1, size_t dim0 );
    ScMatrix( const ScMatrix<T> &ot ) : m_mem( ot.m_mem ){  }
    ScMatrix<T> &  operator = ( const ScMatrix<T> &ot ) { m_mem = ot.m_mem; return *this; }

#ifdef QXPACK_SC_CXX11
    ScMatrix( ScMatrix<T> &&ot )
    { m_mem.movePtrFrom( ot.m_mem ); }

    ScMatrix &  operator = ( ScMatrix<T> &&ot )
    { if ( this != &ot ) { m_mem.movePtrFrom( ot.m_mem ); } return *this; }
#endif

    inline bool   isNull() const { return m_mem.isNull(); }
    inline size_t   dim1() const { return ( m_mem.isValid() ? infoHdr()->dim[1] : 0 ); }
    inline size_t   dim0() const { return ( m_mem.isValid() ? infoHdr()->dim[0] : 0 ); }
    inline const T* constData() const { return ( m_mem.isValid() ? dataPtr() : nullptr ); }

    //! @since 0.1.0
    //! @note make a copy if shared with other
    inline bool  dupIfShared() { return m_mem.dupIfShared(); }

    //! @since 0.1.0
    //! @brief return the data pointer directly and do a copy if shared
    //! @warning this function DO A DEEP COPY if shared!
    inline T*    dataCow() { if ( m_mem.isValid()){ m_mem.dupIfShared(); return dataPtr(); } else { return nullptr; }}

    //! @since 0.1.0
    //! @brief make a copy of current object
    inline ScMatrix<T>  copy() const { ScMatrix<T> mat; mat.m_mem = m_mem.copy(); return mat; }

    template <typename Ti>
    inline ElemVisitor operator[] ( const Ti &i )
    {
        QXPACK_SC_ASSERT( std::is_integral<Ti>::value, "index must be intergral value!" );
        QXPACK_SC_FATAL( ! m_mem.isNull(), "matrix is empty!" );
        size_t d1 = infoHdr()->dim[1], d0 = infoHdr()->dim[0];
        QXPACK_SC_ASSERT( size_t(i) < d1, "( i = %u, dim1 = %u ) out of dimension index!", size_t(i), d1 );
        return ElemVisitor( d0, dataPtr() + size_t(i) * d0 );
    }


};

// ============================================================================
// ctor
// ============================================================================
template <typename T>
ScMatrix<T> :: ScMatrix ( size_t dim1, size_t dim0 )
{
    QXPACK_SC_ASSERT( dim1 > 0 && dim0 > 0, "bad parameter!" );

    if ( dim1 > 0 && dim0 > 0 ) {
        bool is_cls = std::is_class<T>::value;
        if ( is_cls ) { ScMemBlockHelper::verifyClass<T>(); }

        size_t t_sz = sizeof( T ) * dim0 * dim1 + sizeof( InfoHdr );
        if ( ! is_cls ) {
            m_mem = ScMemBlock( t_sz );
        } else {
            m_mem = ScMemBlock( t_sz,
               []( ScMemBlock::ActID  act_id, void *par, void* rsvd )->bool {
                  return ScMemBlockHelper::userProcT<T, sizeof(InfoHdr)>( act_id, par, rsvd );
               }
            , nullptr );
        }

        InfoHdr *hdr = infoHdr();
        hdr->hdr_sz  = sizeof( InfoHdr ); hdr->fix_step = 2;
        hdr->dim[0]  = dim0; hdr->dim[1] = dim1;
    }
}

}
#endif
