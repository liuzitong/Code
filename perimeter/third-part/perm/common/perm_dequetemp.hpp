#ifndef PERM_DEQUETSTEMP_HPP
#define PERM_DEQUETSTEMP_HPP

#include <mutex> // C++ 11
#include <deque> // C++ 11

namespace FcPerm {

// /////////////////////////////////////////////////////////
//
// a thread safe deque template
//
// /////////////////////////////////////////////////////////
template <typename T>
class  Deque {
private: 
    std::mutex m_locker; std::deque<T> m_queue;
    void (* m_clear_cb)( T&,void* ); void *m_ctxt;
public :
    Deque ( void (*clearFunc)( T &, void *), void *ctxt );
    virtual ~Deque( );
 
    inline int size( ) { m_locker.lock(); int sz = m_queue.size(); m_locker.unlock(); return sz; }
    void  clear( void(* clearFunc )( T&, void* ), void *ctxt );    
    void  push ( const T& );
    bool  pop  ( T& );
}; 

// ==========================================================
// CTOR
// ==========================================================
template < typename T >
    Deque<T> :: Deque ( void (*clearFunc)( T&, void *), void *ctxt )
{
    m_clear_cb = clearFunc; m_ctxt = ctxt;
}

// ==========================================================
// DTOR
// ==========================================================
template < typename T >
    Deque<T> :: ~Deque( )
{
    if ( m_clear_cb != nullptr ) {
        this->clear( m_clear_cb, m_ctxt );
    }
}

// ==========================================================
// clear all elements 
// ==========================================================
template <typename T>
void   Deque<T> :: clear( void(* clearFunc )( T&, void* ), void *ctxt )
{
    if ( clearFunc == nullptr ) { return; }

    m_locker.lock();
    while ( ! m_queue.empty()) {
        (* clearFunc )( m_queue.front(), ctxt ); m_queue.pop_front();
    }
    m_queue = std::deque<T>();
    m_locker.unlock();
}

// ==========================================================
// push an element into queue
// ==========================================================
template <typename T >
void   Deque<T> :: push ( const T& t )
{
    m_locker.lock( );
    m_queue.push_back( t );
    m_locker.unlock( );
}

// ===========================================================
// pop an element if possible
// ===========================================================
template <typename T >
bool   Deque<T> :: pop ( T &t )
{
    bool is_pop = false;
    m_locker.lock();
    if ( ! m_queue.empty() ) {
        t = m_queue.front(); m_queue.pop_front(); 
        is_pop = true;   
    }
    m_locker.unlock();
    return is_pop;
}


}

#endif 
