#ifndef QXPACK_IC_STACKTEMP_HPP
#define QXPACK_IC_STACKTEMP_HPP

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include <atomic> // C++ 11
#include <thread> // C++ 11

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//                    [ thread safe class ]
/*!
 * @brief this class provide a linked Node stack template
 * @note  this class do not manage memory, the Node class should be managed by user
 */
// ////////////////////////////////////////////////////////////////////////////
template <typename T>
class IcNodeStackTemp {
public :
    // node class is the inner data structure
    class Node {
    private: Node *m_next; T m_dat;
    public :
         explicit Node() { m_next = nullptr; }
         Node( const T &d ) { m_next = nullptr; m_dat = d; }
         inline T data( ) const { return m_dat; }
         inline void setData( const T &d ) { m_dat = d; }
    friend class IcNodeStackTemp<T>;
    };

private:
    std::atomic< Node* >  m_root;
    void (*m_clear_func)( Node*, void*); void *m_ctxt;
    Node  m_tmp;

protected:
    Node*  tryReadRoot( IcLCG & );
public :
    //! ctor
    IcNodeStackTemp ( void (*clearFunc)( Node*, void*), void *ctxt = nullptr );

    //! dtor
    ~IcNodeStackTemp( );

    //! clear all nodes
    void  clear( );

    //! check if it is empty
    inline bool isEmpty( ) { return m_root.load( std::memory_order_seq_cst ) == nullptr; }

    //! pop an element node, if empty, return null.
    Node*  pop( );

    //! push an element pointer
    void   takeAndPush( Node* );
};

// ============================================================================
// ctor
// ============================================================================
template <typename T>
IcNodeStackTemp<T> :: IcNodeStackTemp( void (*clearFunc)( Node*, void *), void *ctxt )
{
    m_root.store( nullptr, std::memory_order_seq_cst );
    m_clear_func = clearFunc;
    m_ctxt = ctxt;
}

// ============================================================================
// dtor
// ============================================================================
template < typename T >
IcNodeStackTemp<T> :: ~IcNodeStackTemp ( )
{ clear(); }

// ============================================================================
// clear all elements
// ============================================================================
template < typename T >
void  IcNodeStackTemp<T> :: clear ( )
{
    while ( ! isEmpty() && m_clear_func != nullptr ) {
        Node* n = pop();
        if ( n != nullptr ) {
            m_clear_func( n, m_ctxt );
        }
    }
}

// ============================================================================
// try read the root node
// ============================================================================
template < typename T >
typename IcNodeStackTemp<T>::Node*  IcNodeStackTemp<T> :: tryReadRoot( IcLCG &lcd )
{
    Node *n = nullptr;
    int cnt = static_cast<int>( lcd.value() % 100 ) + 50;

    // ------------------------------------------------------------------------
    // n is nullptr -- no nodes in list.
    // n is &m_tmp  -- other thread is working on the root.
    // ------------------------------------------------------------------------
    do {
        if ( -- cnt < 1 ) {
            std::this_thread::yield();
            cnt = static_cast<int>( lcd.value() % 100 ) + 50;
        }
        n = m_root.load ( std::memory_order_seq_cst );
        if ( n == nullptr ||  n != &m_tmp ) { break; }
    } while ( true );

    return n;
}

// ============================================================================
// pop a node from list
// ============================================================================
template <typename T>
typename IcNodeStackTemp<T>::Node*  IcNodeStackTemp<T> :: pop( )
{
    Node *n = nullptr; IcLCG lcd;
    do {
        // --------------------------------------------------------------------
        // try get a valid node
        // --------------------------------------------------------------------
        n = tryReadRoot( lcd );
        if ( n == nullptr ) { break; }

        // --------------------------------------------------------------------
        // mark root as busy ( point to tmp node )
        // 1) this is needed because below scenario:
        //    thread-a get the node from root, and paused.
        //    thread-b get the same node from root, and read the next, make root point to next.
        //    thread-b returned, user deleted the node.
        //    thread-a runs, it try to read next, memory access failed!
        // --------------------------------------------------------------------
        if ( m_root.compare_exchange_strong( n, & m_tmp, std::memory_order_seq_cst )) {
            Node *next = n->m_next; Node *tmp_ptr = & m_tmp;
            while ( ! m_root.compare_exchange_strong( tmp_ptr, next )){ } // restore to next..
            n->m_next = nullptr;
            break;
        }
    } while ( true );

    return n;
}

// ============================================================================
// push a node to list, the ownership will be taked.
// caller should not to delete the node.
// ============================================================================
template < typename T >
void  IcNodeStackTemp<T> :: takeAndPush ( Node* t )
{
    Node *n = nullptr; IcLCG lcd;
    do {
        n = tryReadRoot( lcd );
        t->m_next = n;
        if ( m_root.compare_exchange_strong( n, t, std::memory_order_seq_cst )) {
            break;
        }
    } while ( true );
}

}

#endif
