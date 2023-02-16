// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_heapvectortemp
  @author  night wing
  @date    2018/09
  @brief   A portable HeapVector
  @par     history
  @verbatim
          <author>   <time>   <version>  <desc>
         nightwing  2018/08   0.1.0     build
         nigthwing  2018/09   0.2.0     fixed
         nightwing  2020/09   0.3.0     add shrinkIf()
  @endverbatim
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_HEAPVECTOR_TEMP_HPP
#define QXPACK_IC_HEAPVECTOR_TEMP_HPP

#include <cstdint>
#include <vector>
#include <mutex>
#include <memory>
#include <string>
#include <cassert>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// heap vector template
// NOTE: multi-thread supported. and call clear() before DTOR
//
// Optimizer hints: \n
//   1) use T's pointer as element for class/structure
//   2) call shrinkIf() for memory space saving.
// ////////////////////////////////////////////////////////////////////////////
template <typename T>
class IcHeapVectorLess {
public:
    inline bool  operator( ) ( const T& a, const T& b ) const
    { return ( a < b ); }
};

template <typename T>
class  IcHeapVectorGreat {
public:
    inline bool  operator( ) ( const T& a, const T& b ) const
    { return ( a > b ); }
};

template <typename T, class Less >
class  IcHeapVector  {
private:
    std::vector<T>        m_data;
    std::recursive_mutex  m_locker;
    Less   m_is_less;
    void (* m_clear_func )( T&, void*); void *m_clear_ctxt;
    bool   m_use_lock;
protected:
    inline void  _lock()   { if ( m_use_lock ) { m_locker.lock(); }}
    inline void  _unlock() { if ( m_use_lock ) { m_locker.unlock();}}
    int   _indexOf ( bool(*enumFunc)(const T&,void*), void*ctxt );
    int   _siftUp  ( int i );
    int   _siftDown( int i );
    bool  _rmvAt   ( int i, T& );
public:
    explicit IcHeapVector ( void (*clearFunc)( T &, void *), void *ctxt, int pre_alloc = 0, bool use_locker = true );
    ~IcHeapVector( );

    inline int  size( )    { _lock(); int sz = int( m_data.size()); _unlock(); return sz; }
    inline bool isEmpty()  { _lock(); bool ret = m_data.empty(); _unlock(); return ret; }
    void   clear ( void(*clearFunc )( T&, void*), void *ctxt );
    bool   searh ( bool(*searchFunc)( T&, void*), void *ctxt );
    bool   visit ( int idx, void(*visitFunc)( T&,void*), void*ctxt );
    bool   removeAt  ( int i, T &item );
    bool   removeLast( T &item );
    bool   removeByEnum( bool(*enumFunc)( const T&,void*), void*ctxt, T &item, int *rmv_pos_out = nullptr );
    void   enumAll( void(*enumFunc)( T&,void*), void*ctxt );
    int    insert ( const T &e );

    // since 2020/09
    int    shrinkIf ( int (*CondFunc)( const std::vector<T>&, void* ), void* );
    T      takeTopIf( bool(*CondFunc)( const T&, void* ), void* );
};

// ============================================================================
// CTOR
// ============================================================================
template <typename T, class Less>
    IcHeapVector<T,Less> :: IcHeapVector ( void (*clearFunc)( T &, void *), void *ctxt, int pre_alloc, bool use_locker )
{
    m_clear_func = clearFunc; m_clear_ctxt = ctxt;
    if ( pre_alloc > 0 ) { m_data.reserve( ( pre_alloc + 7 ) / 8 * 8 ); }
    m_use_lock = use_locker;
}

// ============================================================================
// DTOR
// ============================================================================
template <typename T, class Less >
    IcHeapVector<T,Less> :: ~IcHeapVector( )
{
    if ( m_clear_func != nullptr ) {
        this->clear( m_clear_func, m_clear_ctxt );
    }
}

// ============================================================================
// clear all elements
// ============================================================================
template <typename T,class Less>
void  IcHeapVector<T,Less> :: clear(void (*clearFunc)(T &, void *), void *ctxt )
{
    _lock(); //m_locker.lock();

    int full_sz = int( m_data.size() );
    if ( clearFunc != nullptr ) {
        for ( int i = 0; i < full_sz; i ++ ) {
            (* clearFunc )( m_data.at(i), ctxt );
        }
    }
    m_data = std::vector<T>( );

    _unlock(); //m_locker.unlock();
}

// ============================================================================
// find element by enum, return the position [ protected ]
// ============================================================================
template <typename T,class Less>
int   IcHeapVector<T,Less> :: _indexOf( bool(*enumFunc)(const T&,void*), void*ctxt )
{
    int sz = int( m_data.size());  int pos = -1;
    for ( int i = 0; i < sz; i ++ ) {
        if ( enumFunc( m_data.at(i), ctxt )) { pos = i; break; }
    }
    return pos;
}

// ============================================================================
// moving the most minimal item to top
// ============================================================================
template <typename T,class Less>
int IcHeapVector<T, Less>::_siftUp( int i )
{
    T key = m_data[i];

    while ( i > 0 ) {
        // --------------------------------------------------------------------
        // p is the parent of i, must keep p <= key [ mini heap ]
        // --------------------------------------------------------------------
        int p = ( i - 1 ) / 2;
        if ( ! m_is_less( key, m_data.at(p))) { break; }

        // --------------------------------------------------------------------
        // copy [p] to [i], i point to orginal p.
        // --------------------------------------------------------------------
        m_data[i] = m_data[p];
        i = p;
    }
    m_data[i] = key;
    return i;
}

// ============================================================================
// try moving down the item
// ============================================================================
template <typename T,class Less>
int  IcHeapVector<T,Less> :: _siftDown( int i )
{
     T key = m_data.at(i);
     int full_sz = int( m_data.size());

     while ( i < full_sz ) {
     // -----------------------------------------------------------------------
     // c is the left child of i ( i is based 0 )
     // -----------------------------------------------------------------------
         int c = ( i * 2 ) + 1;
         if ( c >= full_sz ) { break; } // already go out of range

     // -----------------------------------------------------------------------
     // compare right child if possible, find the minimal item
     // after this, c is point to the minimal item
     // -----------------------------------------------------------------------
         if ( c + 1 < full_sz &&
             m_is_less( m_data.at(c+1), m_data.at(c) )
         ) { c ++; }

     // -----------------------------------------------------------------------
     // check if key less than child. if so, go out.
     // -----------------------------------------------------------------------
         if ( ! m_is_less( m_data.at(c), key )) { break; }

     // -----------------------------------------------------------------------
     // copy child element to i, i point to child.
     // -----------------------------------------------------------------------
         m_data[i] = m_data.at(c);
         i = c;
     }
     m_data[i] = key;

     return i;
}

// ============================================================================
// remove item at spec. position
// ============================================================================
template <typename T, class Less>
bool  IcHeapVector<T,Less> :: _rmvAt( int i, T &item )
{
   if ( i >= 0  &&  i < int( m_data.size())) {
        item = m_data.at(i);
        if ( i != int( m_data.size() - 1 )) { // i is not the last one.
            m_data[i] = m_data.at( m_data.size() - 1 ); // move the last one to specified pos. i
        }
        m_data.pop_back();                              // decrease the total size
        if ( i < int( m_data.size()) ) { // i is in range. if remove last one, do nothing.
            this->_siftDown( i );
            this->_siftUp( i );       // NOTE: do not remove this!
        }
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// search all elements until user found a item that match required
// ============================================================================
template <typename T,class Less>
bool  IcHeapVector<T,Less> :: searh ( bool(*searchFunc)( T&, void*), void *ctxt )
{
    bool is_found = false;
    if ( searchFunc == nullptr ) { return false; }

    _lock();  //m_locker.lock();

    int sz  = int( m_data.size());
    for ( int x = 0; x < sz; x ++ ) {
        if ( ( is_found = (* searchFunc )( m_data.at(x), ctxt ))) {
            break;
        }
    }

    _unlock(); //m_locker.unlock();

    return is_found;
}

// ============================================================================
// visit every element in heap
// ============================================================================
template <typename T,class Less>
bool  IcHeapVector<T,Less> :: visit ( int idx, void(*visitFunc)( T&,void*), void*ctxt )
{
    bool is_found = false;
    if ( visitFunc == nullptr ) { return false; }

    _lock(); //m_locker.lock();

    if ( idx >= 0  && idx < int( m_data.size()) ) {
        (* visitFunc )( m_data.at(idx), ctxt );
        is_found = true;
    }

    _unlock(); // m_locker.unlock();
    return is_found;
}

// ============================================================================
// remove element at specified position
// ============================================================================
template <typename T,class Less>
bool   IcHeapVector<T,Less> :: removeAt( int i, T &item )
{
    bool is_rmv = false;
    _lock();    //m_locker.lock();
    is_rmv = _rmvAt( i, item );
    _unlock();  //m_locker.unlock();
    return is_rmv;
}

// ============================================================================
// remove the last element
// ============================================================================
template <typename T, class Less>
bool   IcHeapVector<T,Less> :: removeLast( T &item )
{
    bool is_rmv = false;
    _lock();   // m_locker.lock();

    if ( ! m_data.empty() ) { // nw: 2019/05/10, old is if ( m_data.size() > 0 ) {
        item = m_data.at( m_data.size() - 1 ); // the last one
        m_data.pop_back();
        is_rmv = true;
    }

    _unlock();  //m_locker.unlock();
    return is_rmv;
}

// ============================================================================
// remove element by enum result
// ============================================================================
template <typename T,class Less>
bool  IcHeapVector<T,Less> :: removeByEnum( bool(*enumFunc)( const T&,void*), void*ctxt, T &item, int *rmv_pos_out )
{
    bool is_rmv = false;
    _lock();  // m_locker.lock();

    int pos = this->_indexOf( enumFunc, ctxt );
    if ( pos >= 0 ) { this->removeAt( pos, item ); is_rmv = true; }

    _unlock(); //m_locker.unlock();
    if ( is_rmv && rmv_pos_out != nullptr ) { *rmv_pos_out = pos; }
    return is_rmv;
}

// ============================================================================
// enum all elements ( nw: 2018/08/28 added )
// ============================================================================
template <typename T,class Less>
void   IcHeapVector<T,Less> :: enumAll( void(*enumFunc)( T&,void*), void*ctxt )
{
    _lock(); //m_locker.lock();

    size_t e_num = m_data.size();
    for ( size_t i = 0; i < e_num; i ++ ) {
        (* enumFunc )( m_data.at(i), ctxt );
    }

    _unlock(); //m_locker.unlock();
}


// ============================================================================
// insert a new element into heap
// ============================================================================
template <typename T,class Less>
int IcHeapVector<T, Less>::insert( const T &e )
{
    _lock(); // m_locker.lock();

    m_data.push_back( e );
    int pos = this->_siftUp( int( m_data.size()) - 1 );

    _unlock();  //m_locker.unlock();
    return pos; // return the current position about insert element
}

// ============================================================================
// [ since 2020/09 ]  shrink memory if condition is meeting
// ============================================================================
template <typename T, class Less>
int  IcHeapVector<T,Less> :: shrinkIf( int (*CondFunc)( const std::vector<T>&, void* ), void *ctxt )
{
    if ( CondFunc == nullptr ) { return -1; }

    _lock();

    int cap_sz = CondFunc( m_data, ctxt );
    int e_sz   = int( m_data.size());

    if ( cap_sz > 0  &&  cap_sz >= e_sz ) {
        std::vector<T> tmp; tmp.reserve( cap_sz ); tmp.resize( e_sz );
        for ( int i = 0; i < e_sz; i ++ ) { tmp[i] = m_data.at(i); }
        m_data.swap( tmp );
    }
    _unlock();

    return cap_sz;
}

// ============================================================================
// [ since 2020/09 ] take the top element if condition meetting
// ============================================================================
template <typename T, class Less>
T    IcHeapVector<T,Less> :: takeTopIf( bool(*CondFunc)( const T&, void* ), void *ctxt )
{
    if ( CondFunc == nullptr ) { return T(); }
    T tmp;

    _lock();
    if ( m_data.size() > 0 ) {
        if ( CondFunc( m_data.at(0), ctxt )) { _rmvAt(0, tmp); }
    }
    _unlock();

    return tmp;
}

}

#endif
