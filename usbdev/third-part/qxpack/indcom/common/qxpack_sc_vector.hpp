// ////////////////////////////////////////////////////////////////////////////
//! @file     qxpack_sc_vector.hpp
//! @brief    this is a portable vector 
//! @author   night wing
//! @date     2020/08
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_SC_VECTOR_HPP
#define QXPACK_SC_VECTOR_HPP

#include "qxpack_sc_memblock.hxx"

// ////////////////////////////////////////////////////////////////////////////
//
//                      vector template 
// fix_step : [0,1]  just only array template
// fix_step : [2,3]  a dyncmatical array template
// ////////////////////////////////////////////////////////////////////////////
namespace QxPack {

template <typename T>
class QXPACK_SC_HIDDEN ScVector QXPACK_SC_FINAL {
private:
    ScMemBlock  m_mem;

protected :
    // fix_step < 2, capa and fix_step is zero
    struct InfoHdr QXPACK_SC_FINAL { size_t hdr_sz, dim, capa, fix_step; };

    inline InfoHdr*  infoHdr() const            { return  reinterpret_cast<InfoHdr*>( const_cast<void*>(m_mem.constData()) ); }
    static InfoHdr*  infoHdr( ScMemBlock &mem ) { return  reinterpret_cast<InfoHdr*>( const_cast<void*>(mem.constData()) ); }
    inline T*        dataPtr() const            { InfoHdr *hdr = infoHdr(); return reinterpret_cast<T*>( reinterpret_cast<uint8_t*>(hdr) + hdr->hdr_sz ); }
    static T*        dataPtr( ScMemBlock &mem )
    {
        InfoHdr *h = reinterpret_cast<InfoHdr*>( const_cast<void*>( mem.constData()) );
        return reinterpret_cast<T*>( reinterpret_cast<uint8_t*>(h) + h->hdr_sz );
    }

    inline InfoHdr*    ensureCapa() { InfoHdr *h = infoHdr(); if ( h->capa == 0 ) { h->capa = h->dim; } return h; }

    inline size_t      estmCapaByAdd( size_t add_num )
    { InfoHdr *h = infoHdr(); return h->capa + ScMemBlockHelper::clamp<size_t>( ( h->capa + add_num + 1 ) / 2, 8, 256 ); }

    inline ScMemBlock  buildMemForClass ( size_t capa )
    { return ScMemBlock( (capa>0?capa:1) * sizeof(T) + sizeof(InfoHdr), & ScVector<T>::userProcT, nullptr ); }

    inline ScMemBlock  buildMemForData  ( size_t capa )
    { return ScMemBlock( (capa>0?capa:1) * sizeof(T) + sizeof(InfoHdr) ); }


    // ////////////////////////////////////////////////////////////////////////
    //                  vector's user proc ( used for class )
    // ////////////////////////////////////////////////////////////////////////
    static bool  userProcT( ScMemBlock::ActID act_id, void *act_par, void* )
    {
        QXPACK_SC_ASSERT( act_par != nullptr, "parameter pointer is null!" );
        switch( act_id ) {
        case ScMemBlock::ActID_Init : {
            // ----------------------------------------------------------------
            // just init header only
            // ----------------------------------------------------------------
            auto *par  = reinterpret_cast<ScMemBlock::ActInitParam*>( act_par );
            InfoHdr* h = reinterpret_cast<InfoHdr*>( par->data );
            h->hdr_sz  = sizeof(InfoHdr); h->dim = 0; h->fix_step = 3;
            h->capa    = ( par->data_sz - sizeof(InfoHdr)) / sizeof(T);

        } break;

        case ScMemBlock::ActID_Destroy : {
            // ----------------------------------------------------------------
            // destroy all T object, just deinit all object that
            // ----------------------------------------------------------------
            auto *par  = reinterpret_cast<ScMemBlock::ActDestroyParam*>( act_par );
            InfoHdr *h = reinterpret_cast<InfoHdr*>( par->data );
            T *dp      = reinterpret_cast<T*>( ScMemBlockHelper::ptrAddHdr( par->data, sizeof(InfoHdr)));
            ScMemBlockHelper::deInitT( dp, h->dim );

        } break;

        case ScMemBlock::ActID_InitByCopy : {
            // ----------------------------------------------------------------
            // just copy objects, number is source.dim
            // ----------------------------------------------------------------
            auto *par = reinterpret_cast<ScMemBlock::ActInitByCopyParam*>( act_par );
            InfoHdr *tgt_h = reinterpret_cast<InfoHdr*>( par->tgt_data );
            InfoHdr *src_h = reinterpret_cast<InfoHdr*>( par->src_data );
            T *tgt_dp = reinterpret_cast<T*>( ScMemBlockHelper::ptrAddHdr( par->tgt_data, sizeof(InfoHdr)));
            T *src_dp = reinterpret_cast<T*>( ScMemBlockHelper::ptrAddHdr( par->src_data, sizeof(InfoHdr)));
            tgt_h->hdr_sz   = sizeof( InfoHdr );  tgt_h->dim = src_h->dim;
            tgt_h->capa     = ( par->tgt_sz - sizeof(InfoHdr)) / sizeof(T);
            tgt_h->fix_step = 3;
            ScMemBlockHelper::initByCopyT( tgt_dp, tgt_h->dim, src_dp, src_h->dim );
        } break;

        default: return false;
        }
        return true;
    }

    // ////////////////////////////////////////////////////////////////////////
    //                  for class type template
    //
    // _p_***: function does not check shared state
    // _***  : function must check the shared state
    // ////////////////////////////////////////////////////////////////////////
    // ------------------------------------------------------------------------
    // initialize the class type from a to b
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<std::is_class<Tx>::value>::type  _p_initRange( size_t from, size_t to )
    { T *dp = dataPtr(); for ( size_t i = from; i <= to; i ++ ) { ScMemBlockHelper::initOneT( dp[i] ); }}

    // ------------------------------------------------------------------------
    // deinit the class type from a to b
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<std::is_class<Tx>::value>::type  _p_deinitRange( size_t from, size_t to )
    { T *dp = dataPtr(); for ( size_t i = from; i <= to; i ++ ) { ScMemBlockHelper::deInitOneT( dp[i] ); }}

    // ------------------------------------------------------------------------
    // move segment to back
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    static typename std::enable_if<std::is_class<Tx>::value>::type
    _p_moveToBack( T *dp, size_t num, size_t dist )
    {
        if ( dist == 0 ) { return; }
        T *dst = dp + num + dist - 1;  T *src = dp + num - 1;
        for ( size_t i = 0; i < num; i ++ ) {
            ScMemBlockHelper::takeOneByMoveT( *( dst -- ), *( src -- ));
        }
    }

    // ------------------------------------------------------------------------
    // move segment to front
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    static typename std::enable_if<std::is_class<Tx>::value>::type
    _p_moveToFront( T *dp, size_t num, size_t dist )
    {
        if ( dist == 0 ) { return; }
        T *dst = dp - dist;  T *src = dp;
        for ( size_t i = 0; i < num; i ++ ) {
            ScMemBlockHelper::takeOneByMoveT( dst[i], src[i] );
        }
    }

    // ------------------------------------------------------------------------
    // create the memory by item number. In Class Type, it used userProcT()
    // NOTE: the header->dim is 0 after this call.
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<std::is_class<Tx>::value>::type  _createMemByNum ( size_t n )
    { m_mem.makeNull(); m_mem = buildMemForClass( n ); }

    // ------------------------------------------------------------------------
    // expand memory and make memory exclusive
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<std::is_class<Tx>::value>::type  _expandAndExclMem( )
    {
        InfoHdr *old_h = infoHdr();
        if ( old_h->dim >= old_h->capa ) {
            ScMemBlock tgt_mem = buildMemForClass( estmCapaByAdd(1) );
            ScMemBlockHelper::initByCopyT( ScVector<T>::dataPtr( tgt_mem ), old_h->dim, dataPtr(), old_h->dim );
            ScVector<T>::infoHdr( tgt_mem )->dim = old_h->dim;
            m_mem.movePtrFrom( tgt_mem ); // pick pointer
        } else {
            m_mem.dupIfShared();          // objects copiedin InitByCopy()
        }
    }

    // ------------------------------------------------------------------------
    // just build one elem vector
    // NOTE: this is called by pushXXX()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<std::is_class<Tx>::value>::type  _buildOne( const T &v )
    { _createMemByNum( 1 ); infoHdr()->dim = 1; ScMemBlockHelper::initOneByCopyT( *dataPtr(), v ); }

    // ------------------------------------------------------------------------
    // push an element into tail
    // NOTE: _pushBack2() used by push_back()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<std::is_class<Tx>::value>::type  _pushBack2( const T &v )
    {
        _expandAndExclMem(); // dim always point to empty position.
        ScMemBlockHelper::initOneByCopyT( dataPtr()[ infoHdr()->dim ++ ], v );
    }

    // ------------------------------------------------------------------------
    // pop an element form tail
    // NOTE: if the vector is shared, it will be copied.
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename  std::enable_if<std::is_class<Tx>::value>::type  _popBack2()
    {
        if ( infoHdr()->dim > 0 ) {
            m_mem.dupIfShared(); // objects copied in InitByCopy()
            ScMemBlockHelper::deInitOneT( dataPtr()[ -- infoHdr()->dim ] ); // data[dim] must be a empty
        }
    }

    // ------------------------------------------------------------------------
    // push an element into front
    // NOTE: _pushFront2() used by pop_front()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename  std::enable_if<std::is_class<Tx>::value>::type  _pushFront2( const T &v )
    {
        _expandAndExclMem(); InfoHdr *new_h = infoHdr(); T *dp = dataPtr();
        ScMemBlockHelper::initOneT( dp[ new_h->dim ] );
        _p_moveToBack( dp, new_h->dim, 1 );
        ScMemBlockHelper::deInitOneT( dp[0] );
        ScMemBlockHelper::initOneByCopyT( dp[0], v );
        ++ new_h->dim;
    }

    // ------------------------------------------------------------------------
    // pop the element from front
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename  std::enable_if<std::is_class<Tx>::value>::type  _popFront2( )
    {  
        if ( infoHdr()->dim > 0 ) {
            m_mem.dupIfShared(); T *dp = dataPtr(); InfoHdr *new_h = infoHdr();
            ScMemBlockHelper::deInitOneT( dp[0] );
            ScMemBlockHelper::initOneT  ( dp[0] );
            _p_moveToFront( dp + 1, -- new_h->dim, 1 );
            ScMemBlockHelper::deInitOneT( dp[ new_h->dim ] );
        }
    }

    // ------------------------------------------------------------------------
    // clear all elements ( always drop current content !)
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename  std::enable_if<std::is_class<Tx>::value>::type  _clear2()
    {
         InfoHdr *old_h = infoHdr();
         if ( old_h->dim > 0 ) {
             ScMemBlock tgt_mem = buildMemForClass( old_h->capa / 2 );
             m_mem.movePtrFrom( tgt_mem );
             infoHdr()->dim = 0; // cleared
         }
    }

    // ------------------------------------------------------------------------
    // grow element number, the size less capacity()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<std::is_class<Tx>::value>::type  _growSizeLeCap( size_t tgt_sz )
    {
        // always try duplicate memory if it shared with others
        m_mem.dupIfShared();  InfoHdr *new_h = infoHdr();
        QXPACK_SC_ASSERT( tgt_sz > new_h->dim && tgt_sz <= new_h->capa, "target size should in range ( dim, cap ] !" );
        _p_initRange( new_h->dim, tgt_sz - 1 );
        new_h->dim = tgt_sz;
    }

    // ------------------------------------------------------------------------
    // grow element number, the size greater than capacity()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<std::is_class<Tx>::value>::type  _growSizeGtCap( size_t tgt_sz )
    {
        InfoHdr *old_h = infoHdr();
        QXPACK_SC_ASSERT( tgt_sz > old_h->capa, "target size should greater than capacity!" );
        ScMemBlock tgt_mem = buildMemForClass( estmCapaByAdd( tgt_sz - old_h->capa ) );
        ScMemBlockHelper::initByCopyT( ScVector<T>::dataPtr( tgt_mem ), old_h->dim, dataPtr(), old_h->dim );
        size_t old_dim = old_h->dim;
        m_mem.movePtrFrom( tgt_mem ); // pick pointer, then free self content
        infoHdr()->dim = tgt_sz;
        _p_initRange( old_dim, tgt_sz - 1 );
    }

    // ------------------------------------------------------------------------
    // shrink element number, the target size > 0 but less than current size
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<std::is_class<Tx>::value>::type  _shrinkSizeGt0( size_t tgt_sz )
    {
        QXPACK_SC_ASSERT( ( tgt_sz > 0 ) && ( tgt_sz < infoHdr()->dim ), "target size should in range (0,h->dim)." );
        m_mem.dupIfShared(); T *dp = dataPtr(); InfoHdr *new_h = infoHdr();
        ScMemBlockHelper::deInitT( &dp[ tgt_sz ], new_h->dim - tgt_sz );
        new_h->dim = tgt_sz;
    }

    // ------------------------------------------------------------------------
    // insert element, before the position ( dim < capa )
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<std::is_class<Tx>::value>::type  _insBefore( size_t pos, const T &v )
    {
        InfoHdr *old_h = infoHdr();
        QXPACK_SC_ASSERT( pos > 0  &&  pos < old_h->dim,  "position is not in range!" );   // cond 2: position in range [ 0, dim )

        _expandAndExclMem(); T *dp = dataPtr(); InfoHdr *new_h = infoHdr();
        ScMemBlockHelper::initOneT( dp[ new_h->dim ] );
        _p_moveToBack( &dp[ pos ], new_h->dim - pos, 1 );
        ScMemBlockHelper::deInitOneT( dp[ pos ]);
        ScMemBlockHelper::initOneByCopyT( dp[ pos ], v );
        ++ new_h->dim;
    }

    // ------------------------------------------------------------------------
    // remove element, spec. position ( dim < capa )
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<std::is_class<Tx>::value>::type _rmvAt( size_t pos )
    {
        InfoHdr *old_h = infoHdr();
        QXPACK_SC_ASSERT( old_h->dim > 0, "vector is empty!");
        QXPACK_SC_ASSERT( pos > 0 && pos < old_h->dim, "position is not in range!");
        if ( pos >= old_h->dim - 1 ) { pos = old_h->dim - 1; }

        m_mem.dupIfShared(); T *dp = dataPtr(); InfoHdr *new_h = infoHdr();
        ScMemBlockHelper::deInitOneT( dp[ pos ] );
        ScMemBlockHelper::initOneT  ( dp[ pos ] );
        _p_moveToFront( &dp[pos+1], new_h->dim - pos - 1, 1 );
        ScMemBlockHelper::deInitOneT( dp[ -- new_h->dim ] );
    }


    // ////////////////////////////////////////////////////////////////////////
    //                  for non-class data type
    // ////////////////////////////////////////////////////////////////////////
    // ------------------------------------------------------------------------
    // initialize the type
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<!std::is_class<Tx>::value>::type  _p_initRange( size_t from, size_t to )
    { T *dp = dataPtr(); for ( size_t i = from; i <= to; i ++ ) { dp[i] = T(); }}

    // ------------------------------------------------------------------------
    // deinit the class type from a to b
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<!std::is_class<Tx>::value>::type  _p_deinitRange( size_t from, size_t to )
    { QXPACK_SC_UNUSED(from); QXPACK_SC_UNUSED(to); /* need not to do anything */ }

    // ------------------------------------------------------------------------
    // move data member to back
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    static typename std::enable_if<!std::is_class<Tx>::value>::type
    _p_moveToBack( T *dp, size_t num, size_t dist )
    { if ( dist > 0 ) { std::memmove( dp + dist, dp, num * sizeof(T)); }}

    // ------------------------------------------------------------------------
    // move data member to front
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    static typename std::enable_if<!std::is_class<Tx>::value>::type
    _p_moveToFront( T *dp, size_t num, size_t dist )
    { if ( dist > 0 ) { std::memmove( dp - dist, dp, num * sizeof(T)); }}

    // -----------------------------------------------------------------------
    // create the memory by spec. item number. In non-class type, just create mem.
    // and fill header.
    // -----------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<!std::is_class<Tx>::value>::type  _createMemByNum ( size_t n )
    {
        m_mem.makeNull(); m_mem = buildMemForData(n);
        InfoHdr *new_h = infoHdr();
        new_h->capa = n; new_h->dim = 0; new_h->fix_step = 3; new_h->hdr_sz = sizeof(InfoHdr);
    }

    // ------------------------------------------------------------------------
    // expand memory and make memory exclusive
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<!std::is_class<Tx>::value>::type  _expandAndExclMem()
    {
        InfoHdr *old_h = infoHdr();
        if ( old_h->dim >= old_h->capa ) {
            size_t capa = estmCapaByAdd(1);
            m_mem.resize( capa * sizeof(T) + sizeof( InfoHdr ));
            infoHdr()->capa = capa;
        } else {
            m_mem.dupIfShared();
        }
    }

    // ------------------------------------------------------------------------
    // just build one elem vector
    // NOTE: this is called by pushXXX()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<!std::is_class<Tx>::value>::type  _buildOne( const T &v )
    { _createMemByNum( 1 ); dataPtr()[0] = v; infoHdr()->dim = 1; }

    // ------------------------------------------------------------------------
    // push an element into tail
    // NOTE: _pushBack2() used by push_back()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<!std::is_class<Tx>::value>::type  _pushBack2( const T &v )
    { _expandAndExclMem(); dataPtr()[ infoHdr()->dim ++ ] = v; }

    // ------------------------------------------------------------------------
    // pop an element form tail
    // NOTE: if the vector is shared, it will be copied.
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<!std::is_class<Tx>::value>::type  _popBack2()
    {
        if ( infoHdr()->dim > 0 ) {
            m_mem.dupIfShared(); -- infoHdr()->dim;
        }
    }

    // ------------------------------------------------------------------------
    // push an element into front
    // NOTE: _pushFront2() used by pop_front()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    inline typename std::enable_if<!std::is_class<Tx>::value>::type  _pushFront2( const T &v )
    {
        _expandAndExclMem(); T *dp = dataPtr(); InfoHdr *new_h = infoHdr();
        _p_moveToBack( dp, new_h->dim, 1 );
        dp[0] = v; ++ new_h->dim;
    }

    // ------------------------------------------------------------------------
    // pop the element from front
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<!std::is_class<Tx>::value>::type  _popFront2()
    {
        if ( infoHdr()->dim > 0 ) {
            m_mem.dupIfShared();
            _p_moveToFront( dataPtr() + 1, -- infoHdr()->dim, 1 );
        }
    }

    // ------------------------------------------------------------------------
    // clear all elements
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename  std::enable_if<!std::is_class<Tx>::value>::type  _clear2()
    {
        InfoHdr *old_h = infoHdr();
        if ( old_h->dim > 0 ) {
             ScMemBlock tgt_mem = buildMemForData( old_h->capa / 2 );
             m_mem.movePtrFrom( tgt_mem );
             infoHdr()->dim = 0;
        }
    }   

    // ------------------------------------------------------------------------
    // grow number, the size less capacity()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<!std::is_class<Tx>::value>::type  _growSizeLeCap( size_t tgt_sz )
    {
        m_mem.dupIfShared(); InfoHdr *new_h = infoHdr();
        QXPACK_SC_ASSERT( tgt_sz > new_h->dim && tgt_sz <= new_h->capa, "target size should in range ( dim, cap ] !" );
        _p_initRange( new_h->dim, tgt_sz - 1 );
        new_h->dim = tgt_sz;
    }

    // ------------------------------------------------------------------------
    // grow number, the size greater than capacity()
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<!std::is_class<Tx>::value>::type  _growSizeGtCap( size_t tgt_sz )
    {
        InfoHdr *old_h = infoHdr();
        QXPACK_SC_ASSERT( tgt_sz > old_h->capa, "target size should greater than capacity!" );
        ScMemBlock tgt_mem = buildMemForData( estmCapaByAdd( tgt_sz - old_h->capa ) );
        size_t old_dim = old_h->dim;
        std::memcpy( ScVector<T>::dataPtr( tgt_mem ), dataPtr(), sizeof(T) * old_h->dim );
        m_mem.movePtrFrom( tgt_mem );
        infoHdr()->dim = tgt_sz;
        _p_initRange( old_dim, tgt_sz - 1 );
    }

    // ------------------------------------------------------------------------
    // shrink the member
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<!std::is_class<Tx>::value>::type  _shrinkSizeGt0( size_t tgt_sz )
    {
        QXPACK_SC_ASSERT( ( tgt_sz > 0 ) && ( tgt_sz < infoHdr()->dim ), "target size should in range (0,h->dim)." );
        m_mem.dupIfShared();
        infoHdr()->dim = tgt_sz;
    }

    // ------------------------------------------------------------------------
    // insert element, before the position ( dim < capa )
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<!std::is_class<Tx>::value>::type  _insBefore( size_t pos, const T &v )
    {
        InfoHdr *old_h = infoHdr();
        QXPACK_SC_ASSERT( pos < old_h->dim,  "position is not in range!" );   // cond 2: position in range [ 0, dim )

        _expandAndExclMem(); T *dp = dataPtr(); InfoHdr *new_h = infoHdr();
        _p_moveToBack( &dp[ pos ], new_h->dim - pos, 1 );
        dp[ pos ] = v; ++ new_h->dim;
    }

    // ------------------------------------------------------------------------
    // remove element, spec. position
    // ------------------------------------------------------------------------
    template <typename Tx = T>
    typename std::enable_if<!std::is_class<Tx>::value>::type _rmvAt( size_t pos )
    {
        InfoHdr *old_h = infoHdr();
        QXPACK_SC_ASSERT( old_h->dim > 0, "vector is empty!");
        QXPACK_SC_ASSERT( pos > 0 && pos < old_h->dim, "position is not in range!");
        if ( pos >= old_h->dim - 1 ) { pos = old_h->dim - 1; }

        m_mem.dupIfShared(); T *dp = dataPtr(); InfoHdr *new_h = infoHdr();
        _p_moveToFront( &dp[ pos + 1 ], new_h->dim - pos - 1, 1 );
        -- new_h->dim;
    }

public :
    explicit ScVector() {}
    ~ScVector() { }
    ScVector( size_t dim );
    ScVector( const ScVector<T> &ot ) : m_mem( ot.m_mem ) { }
    ScVector<T>& operator = ( const ScVector<T> &ot ) { m_mem = ot.m_mem; return *this; }

#ifdef QXPACK_SC_CXX11
    ScVector( ScVector<T> &&ot ) { m_mem.movePtrFrom( ot.m_mem ); }
    ScVector<T> & operator = ( ScVector<T> &&ot )
    { if ( this != &ot ) { m_mem.movePtrFrom( ot.m_mem ); } return *this; }
#endif

    // ========================================================================
    // fix_step < 2, compat functions
    // ========================================================================
    inline bool     isNull() const    { return m_mem.isNull(); }   
    inline size_t   dim()    const    { return ( m_mem.isValid() ? infoHdr()->dim : 0 ); }
    inline const T* constData() const { return ( m_mem.isValid() ? dataPtr() : nullptr ); }
    inline bool     dupIfShared()     { return m_mem.dupIfShared(); }
    inline T*       dataCow()         { if ( m_mem.isValid() ) { m_mem.dupIfShared(); return dataPtr(); } else { return nullptr; }; }
    inline ScVector<T> copy() const   { ScVector<T> vec; vec.m_mem = m_mem.copy(); return vec; }

    // ------------------------------------------------------------------------
    // override the operator[], user can read the member by array index.
    // NOTE: Readonly version do not check whether this vector is shared.
    // NOTE: this is a readonly version, use code like below
    //     ScVector<int> vec(10);
    //     const ScVector<int> &vec_const = vec;  // MAKE SURE use 'const reference' !
    //     const int & member = vec_const[1];  // first method to get value reference
    //           int   mem_v  = vec_const[2];  // second method to copy value from array
    // ------------------------------------------------------------------------
    template <typename Ti>
    inline const T&  operator[]( const Ti &i ) const
    {
        QXPACK_SC_STATIC_ASSERT( std::is_integral<Ti>::value, "index must be intergral value!" );
        QXPACK_SC_FATAL( m_mem.isValid(), "vector is null!" );
        size_t d = infoHdr()->dim;
        QXPACK_SC_FATAL( i >= 0 && i < Ti(d), "( i = %d, dim = %u ) out of vector index!", int(i), d );
        return *( dataPtr() + size_t(i) );
    }

    // ------------------------------------------------------------------------
    // override the operator[], user can read/write the member by array index.
    // NOTE: this version should check whether this vector is shared, if so, copy.
    // NOTE: use code like below
    //       ScVector<int>  vec(10)
    //       int& m1 = vec[1];    // reference access
    //       int  m2 = vec[2];    // value copy access
    // ------------------------------------------------------------------------
    template <typename Ti>
    inline T&  operator[]( const Ti &i )
    {
        QXPACK_SC_STATIC_ASSERT( std::is_integral<Ti>::value, "index must be intergral value!" );
        QXPACK_SC_FATAL( m_mem.isValid(), "vector is null!" );
        size_t d = infoHdr()->dim;
        QXPACK_SC_FATAL( i >= 0 && i < Ti(d), "( i = %d, dim = %u ) out of vecotr index!", int(i), d );
        return *( dataCow() + size_t(i) );
    }

    // ========================================================================
    // fix_step == 2
    // ========================================================================
    inline void    makeNull()       { m_mem.makeNull(); }
    inline bool    isValid()  const { return m_mem.isValid(); }
    inline size_t  size()     const { return ( m_mem.isValid() ? infoHdr()->dim : 0 ); }
    inline size_t  capacity() const { return ( m_mem.isValid() ? (const_cast<ScVector<T>*>(this))->ensureCapa()->capa : 0 ); }

    // ========================================================================
    // fix_step == 3
    // ========================================================================
    inline const ScMemBlock &  constMemBlockRef() const { return m_mem; }  

    inline bool    empty() const { return ( m_mem.isValid() ? infoHdr()->dim == 0 : true ); }

    inline size_t  max_size() const { return SIZE_MAX; } // just a hint...

    inline void    clear() { if ( m_mem.isValid()) { _clear2(); }}

           void    reserve( size_t n )
    {
        if ( m_mem.isNull()) { // just space allocated.
            if ( n > 0 ) { _createMemByNum( n ); }
        } else {
            if ( infoHdr()->capa < n ) { m_mem.resize( n ); }
        }
    }

    // ------------------------------------------------------------------------
    // resize the element number
    // ------------------------------------------------------------------------
    template <typename Ti>
    void    resize( Ti pos )
    {
        QXPACK_SC_STATIC_ASSERT( std::is_integral<Ti>::value, "index must be intergral value!" );
        size_t n = size_t(pos);
        if ( m_mem.isNull()) { // just space allocated.
            if ( n > 0 ) { _createMemByNum( n ); }
        } else {
            InfoHdr *h = infoHdr();           
            if ( n > h->dim ) {
                if ( n <= h->capa ) { _growSizeLeCap( n ); } else { _growSizeGtCap( n ); }
            } else if ( n < h->dim ) {
                if ( n > 0 ) { _shrinkSizeGt0( n ); } else { m_mem = ScMemBlock(); }
            } else { /* n == h->dim, nothing to do */ }
        }
    }

    // ------------------------------------------------------------------------
    // insert a element into vector
    // ------------------------------------------------------------------------
    template <typename Ti>
    void  insert( Ti pos, const T &v )
    {
        QXPACK_SC_STATIC_ASSERT( std::is_integral<Ti>::value, "index must be intergral value!" );
        size_t idx = size_t(pos);
        if ( m_mem.isValid()) {
            InfoHdr *h = infoHdr();
            if ( idx >= h->dim ) { _pushBack2( v );  }
            else if ( idx < 1  ) { _pushFront2( v ); }
            else                 { _insBefore( idx, v ); }
        } else { _buildOne( v ); }
    }

    // ------------------------------------------------------------------------
    // remove the element at spec. position
    // ------------------------------------------------------------------------
    template <typename Ti>
    void  remove( Ti pos )
    {
        QXPACK_SC_STATIC_ASSERT( std::is_integral<Ti>::value, "index must be intergral value!" );
        size_t idx = size_t( pos );
        if ( m_mem.isValid() && infoHdr()->dim > 0 ) {
            InfoHdr *h = infoHdr();
            if ( idx >= h->dim ) { _popBack2( );  }
            else if ( idx < 1  ) { _popFront2( ); }
            else                 { _rmvAt( idx ); }
        }
    }

    inline void  push_back ( const T &v )
    { if ( m_mem.isValid()) { _pushBack2<T>( v ); } else { _buildOne( v ); }}

    inline void  push_front( const T &v )
    { if ( m_mem.isValid()) { _pushFront2<T>( v ); } else { _buildOne( v ); }}

    inline void  pop_back()
    { if ( m_mem.isValid() && infoHdr()->dim > 0 ) { _popBack2(); }}

    inline void  pop_front()
    { if ( m_mem.isValid() && infoHdr()->dim > 0 ) { _popFront2(); }}

    inline const T&  back() const
    {
        QXPACK_SC_FATAL( m_mem.isValid(), "vector is null!");
        QXPACK_SC_FATAL( infoHdr()->dim > 0, "vector is empty!" );
        return dataPtr()[ infoHdr()->dim - 1 ];
    }

    inline const T&  front() const
    {
        QXPACK_SC_FATAL( m_mem.isValid(), "vector is null!");
        QXPACK_SC_FATAL( infoHdr()->dim > 0, "vector is empty!" );
        return *( dataPtr());
    }

    template <typename Ti>
    inline const T&  at( const Ti &i ) const
    {
        QXPACK_SC_STATIC_ASSERT( std::is_integral<Ti>::value, "index must be intergral value!" );
        QXPACK_SC_FATAL( m_mem.isValid(), "vector is null!" );
        size_t d = infoHdr()->dim;
        QXPACK_SC_FATAL( i >= 0 && i < Ti(d), "( i = %d, dim = %u ) out of vector index!", int(i), d );
        return *( dataPtr() + size_t(i) );
    }
};

// ============================================================================
// ctor
// ============================================================================
template <typename T>
ScVector<T> :: ScVector( size_t dim )
{
    QXPACK_SC_ASSERT( dim > 0, "bad parameter!" );
    if ( dim > 0 ) {
        bool is_cls = std::is_class<T>::value;
        if ( is_cls ) { ScMemBlockHelper::verifyClass<T>(); }
        _createMemByNum( dim );
        if ( is_cls ) {
            ScMemBlockHelper::initT<T>( dataPtr(), dim );    
        } else {
            // normal data init.
            T *dp = dataPtr();
            std::memset( dp, 0, sizeof(T) * dim );
        }
        // make it full.
        InfoHdr *h = infoHdr(); h->dim = h->capa;
    }
}


}
#endif 

