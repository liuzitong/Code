// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_queuetemp
  @author  night wing
  @date    2019/04, 2019/05
  @brief   A portable Queue
  @par     history
  @verbatim
          <author>   <time>   <version>  <desc>
         nightwing  2019/04   0.1.0      build
         nightwing  2019/05   0.2.0      fixed. IcNodeQueueTemp::Node
                                         added. IcNodeQueueTemp::blockDequeueUntil
  @endverbatim
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_QUEUETEMP_HPP
#define QXPACK_IC_QUEUETEMP_HPP

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/common/qxpack_ic_global.hxx"
#include <atomic> // C++ 11
#include <thread> // C++ 11

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//                        [  Thread Safe Class ]
/*!
 * @brief this class provide a multi-thread accessable FIFO list
 */
// ////////////////////////////////////////////////////////////////////////////
template <typename T>
class IcNodeQueueTemp {
public :
    class Node {
    private: Node *m_next; T m_dat;
    public :
        Node() { m_next = this; }
        Node( const T &d ) { m_dat = d; }
        inline T     data() { return m_dat; }
        inline T &   dataRef() { return m_dat; } // nw: 2019/05/25 added
        inline Node* setData( const T &d ) { m_dat = d; return this; }
    friend class IcNodeQueueTemp<T>;
    };

private:
   // std::atomic_flag    m_lck_flag = ATOMIC_FLAG_INIT;
    std::atomic_int     m_cntr;
    std::atomic<Node*>  m_tail;
    void (*m_cb_func)( Node*, void* ); void *m_ctxt;
    Node m_tmp;
protected:
    Node*  tryReadTail( IcLCG & );
public :
    //! ctor
    IcNodeQueueTemp ( void (*cbFunc)( Node*, void* ), void *ctxt = nullptr );

    //! dtor
    ~IcNodeQueueTemp( );

    //! clear all nodes
    void  clear( );

    //! check if it is empty
    inline bool isEmpty( ) { return m_tail.load( std::memory_order_seq_cst ) == nullptr; }

    //! pop an element node, if empty, return null.
    Node*  dequeue( bool (* preCheck )( Node*, void* ) = nullptr, void *ctxt = nullptr );

    //!  pop element node until preCheck return false
    //!  nw: 2019/05/25 added
    void   blockDequeueUntil (
        void (* nodeHandle )( Node*, void* ), void *n_ctxt,
        bool (* preCheck   )( Node*, void* ), void *p_ctxt
    );

    //! enum all items
    void   blockEnumAll ( void (*nodeHandle)( Node*, void* ), void *ctxt );

    //! push an element pointer
    void   takeAndEnqueue( Node* );

    //! read out current counter
    //! @note the counter value is a snap value
    int    count( ) { return m_cntr.load( std::memory_order_seq_cst ); }

    QXPACK_IC_DISABLE_COPY( IcNodeQueueTemp )
};

// ============================================================================
// ctor
// ============================================================================
template <typename T>
IcNodeQueueTemp<T> :: IcNodeQueueTemp ( void (*cbFunc)( Node*, void* ), void *ctxt )
{
    m_cb_func = cbFunc; m_ctxt = ctxt;
    m_cntr.store( 0, std::memory_order_seq_cst );
    m_tail.store( nullptr, std::memory_order_seq_cst );
}

// ============================================================================
// dtor
// ============================================================================
template <typename T>
IcNodeQueueTemp<T> :: ~IcNodeQueueTemp( )
{ clear();  }

// ============================================================================
// clear all nodes
// ============================================================================
template <typename T>
void  IcNodeQueueTemp<T> :: clear( )
{
    blockDequeueUntil(
        []( Node* n, void *ctxt ) {
            IcNodeQueueTemp<T> *t_this = reinterpret_cast<IcNodeQueueTemp<T>*>( ctxt );
            if ( t_this->m_cb_func != nullptr ) {
                t_this->m_cb_func( n, t_this->m_ctxt );
            }
        }, this,
        []( Node*,void* ) { return true; }, nullptr
    );
}

// ============================================================================
// try read the tail node
// ============================================================================
template <typename T>
typename IcNodeQueueTemp<T>::Node* IcNodeQueueTemp<T>::tryReadTail( IcLCG &lcd )
{
    Node *n = nullptr;
    int cnt = int( lcd.value() % 253 ) + 50;
    if ( cnt < 1 ) { cnt = 50; } // nw: 20190630 fixed.

    // ------------------------------------------------------------------------
    // n is nullptr -- no nodes in list.
    // n is &m_tmp  -- other thread is working on the root.
    // ------------------------------------------------------------------------
    do {
        if ( -- cnt < 1 ) {
            std::this_thread::yield();
            cnt = int( lcd.value() % 253 ) + 50;
            if ( cnt < 1 ) { cnt = 50; } // nw: 20190630 fixed
        }
        n = m_tail.load ( std::memory_order_seq_cst );
        if ( n == nullptr || n != &m_tmp ) { break; }
    } while ( true );

    return n;
}

// ============================================================================
// enqueue a node
// ============================================================================
template <typename T>
typename IcNodeQueueTemp<T>::Node* IcNodeQueueTemp<T> :: dequeue (
    bool (* preCheck )( Node*, void* ), void *ctxt
) {
    Node *n = nullptr; Node *tmp = &m_tmp; IcLCG lcd;
    do {
        // --------------------------------------------------------------------
        // try get the tail node
        // --------------------------------------------------------------------
        Node *tail = tryReadTail( lcd );
        if ( tail == nullptr ) { n = nullptr; break; }

        // --------------------------------------------------------------------
        // a) lock the tail node ( make it point to tmp )
        // b) get the head node ( tail->next )
        // c) fix the tail node next link.
        // --------------------------------------------------------------------
        if ( m_tail.compare_exchange_strong( tail, tmp, std::memory_order_seq_cst )) {

            // check the head node
            bool is_chk_ok = true;
            if ( preCheck != nullptr ) {
                // ------------------------------------------------------------
                // 1) tail->m_next != tail ( head )
                // 2) tail->m_next == tail ( last node )
                // ------------------------------------------------------------
                is_chk_ok = preCheck( tail->m_next, ctxt );
            }

            // now try to handle the head node
            if ( is_chk_ok ) {
                m_cntr.fetch_sub(1);
                if ( tail->m_next != tail ) {  // there are two nodes at least.
                    n = tail->m_next; tail->m_next = n->m_next;
                    while ( ! m_tail.compare_exchange_strong( tmp, tail, std::memory_order_seq_cst ))
                    { }
                } else { // last node
                    n = tail;
                    while ( ! m_tail.compare_exchange_strong( tmp, nullptr, std::memory_order_seq_cst ))
                    { }
                }
                n->m_next = nullptr;
            } else {
                // restore the tail node, unlock it
                while ( ! m_tail.compare_exchange_strong( tmp, tail, std::memory_order_seq_cst )){}
                n = nullptr;
            }
            break;
        }
    } while ( true );

    return n;
}

// ============================================================================
// dequeue node until preCheck return false
// ============================================================================
template <typename T>
void IcNodeQueueTemp<T> :: blockDequeueUntil (
     void (* nodeHandle )( Node*, void* ), void *n_ctxt,
     bool (* preCheck   )( Node*, void* ), void *p_ctxt
) {
    Node *tmp = &m_tmp; IcLCG lcd;
    if ( nodeHandle == nullptr || preCheck == nullptr ) { return; }

    do {
        // --------------------------------------------------------------------
        // try get the tail node, if failed, means no node
        // NOTE: tail may not be nullptr but other threads changed it!!!
        //       so m_tail should check it again!
        // --------------------------------------------------------------------
        Node *tail = tryReadTail( lcd );
        if ( tail == nullptr ) { break; }

        // --------------------------------------------------------------------
        // try lock the tail node, if failed, continue next loop
        // --------------------------------------------------------------------
        if ( ! m_tail.compare_exchange_strong( tail, tmp, std::memory_order_seq_cst )) {
            continue;
        }

        // --------------------------------------------------------------------
        // we has locked tail node, now deque every head node
        // --------------------------------------------------------------------
        int node_handled = 0;  // nw: 20190630 added
        do {
            bool is_chk_ok = preCheck( tail->m_next, p_ctxt );
            if ( ! is_chk_ok ) { break; }

            Node *n = nullptr;
            if ( tail->m_next != tail ) { // exist more than one node
                n = tail->m_next; tail->m_next = n->m_next; n->m_next = nullptr;
            } else {                      // the last node
                n = tail; n->m_next = nullptr; tail = nullptr;
            }
            nodeHandle( n, n_ctxt );
            ++ node_handled;          // nw: 20190630 added
        } while ( tail != nullptr );

        if ( node_handled > 0 ) { m_cntr.fetch_sub( node_handled ); } // nw: 20190630 added
        
        // --------------------------------------------------------------------
        // unlock tail node
        // --------------------------------------------------------------------
        while ( ! m_tail.compare_exchange_strong( tmp, tail, std::memory_order_seq_cst ))
        { }
        break;

    } while( true );
}

// ============================================================================
// since 20200930
// ============================================================================
template <typename T>
void  IcNodeQueueTemp<T> :: blockEnumAll ( void (*nodeHandle)( Node*, void* ), void *n_ctxt )
{
    Node *tmp = &m_tmp; IcLCG lcd;
    if ( nodeHandle == nullptr ) { return; }

    do {
        // --------------------------------------------------------------------
        // try get the tail node, if failed, means no node
        // NOTE: tail may not be nullptr but other threads changed it!!!
        //       so m_tail should check it again!
        // --------------------------------------------------------------------
        Node *tail = tryReadTail( lcd );
        if ( tail == nullptr ) { break; }

        // --------------------------------------------------------------------
        // try lock the tail node, if failed, continue next loop
        // --------------------------------------------------------------------
        if ( ! m_tail.compare_exchange_strong( tail, tmp, std::memory_order_seq_cst )) {
            continue;
        }

        // --------------------------------------------------------------------
        // we has locked tail node, now enum all nodes
        // --------------------------------------------------------------------
        Node *n = tail;
        do {
            n = n->m_next;
            nodeHandle( n, n_ctxt );
        } while ( n != tail ); // already enum all..

        // --------------------------------------------------------------------
        // unlock tail node
        // --------------------------------------------------------------------
        while ( ! m_tail.compare_exchange_strong( tmp, tail, std::memory_order_seq_cst ))
        { }
        break;

    } while( true );
}


// ============================================================================
// enqueue a node
// ============================================================================
template <typename T>
void  IcNodeQueueTemp<T> :: takeAndEnqueue ( Node *t )
{
    Node *n = nullptr; Node *tmp = & m_tmp; IcLCG lcd;
    do {
        n = tryReadTail( lcd );

        // --------------------------------------------------------------------
        // a) lock the tail node ( make tali point to tmp )
        // b) make current tail node link to t
        // c) make t link to head
        // d) replace tail pointer to t
        // --------------------------------------------------------------------
        if ( m_tail.compare_exchange_strong( n, tmp, std::memory_order_seq_cst ) ) {
            m_cntr.fetch_add( 1 ); // the node number is increased.
            if ( n != nullptr ) {  // add the t to tail, t is the last tail node
                Node *nn = n->m_next; n->m_next = t; t->m_next = nn;
            } else {
                t->m_next = t;
            }

            // replace the tail pointer
            while ( ! m_tail.compare_exchange_strong( tmp, t, std::memory_order_seq_cst ))
            {}

            break;
        }
    } while ( true );
}





// ////////////////////////////////////////////////////////////////////////////
//                    [ Thread Safe Class ]
/*!
 * @brief array type queue template
 */
// ////////////////////////////////////////////////////////////////////////////
template <typename T, size_t N>
class IcArrayQueueTemp {
private:
    class IcArrayQueueTempPriv {
    private:
        IcNodeQueueTemp<T*> m_free_list, m_queue;
        typename IcNodeQueueTemp<T*>::Node  m_node[ N + 1 ];
        T  m_slot[ N + 1 ]; // the addition one is used for restore the slot object to default
    friend class IcArrayQueueTemp;

        // ctor
        IcArrayQueueTempPriv( ) :
            m_free_list( nullptr, nullptr ), m_queue( nullptr, nullptr )
        {
            for ( int i = 0; i < N; i ++ ) {
                T* ptr = & m_slot[i];
                m_node[i].setData( ptr );
                m_free_list.takeAndEnqueue( & m_node[i] );
            }
        }

        ~IcArrayQueueTempPriv( )
        {
            m_free_list.clear(); m_queue.clear();
        }
    };

    IcArrayQueueTempPriv *m_priv;
public :
    //! ctor
    IcArrayQueueTemp( );

    //! dtor
    ~IcArrayQueueTemp( );

    //! check if is empty
    inline bool isEmpty( ) { return m_priv->m_queue.isEmpty(); }

    //! clear all objects
    //! @note call this function before should stop other threads to visite enqueue()...
    void  clear( );

    //! enqueue a object
    //! @param d [ in ] the data const ref.
    //! @return true means enqueued
    //! @note  parameter 'd' must implement 'operator ='
    bool  enqueue ( const T &d );

    //! dequeue a object
    //! @param d [out]  the data ref. output
    //! @return true means dequeued; otherwise means empty
    //! @note parameter 'd' must implement 'operator ='
    bool  dequeue ( T &d );

    //! force enqueue the object
    //! @param d        [in]      the data ref. input
    //! @param dropNode [opt_in]  the callback used to handle the dropped Node
    //! @param ctxt     [opt_in]  dropNode callback context pointer
    //! @note this function will call dequeue() to free node if null..
    void  forceEnqueue ( const T &d, void (* dropObj )( T&, void* ) = nullptr, void *ctxt = nullptr );

    QXPACK_IC_DISABLE_COPY( IcArrayQueueTemp )
};

// ============================================================================
// ctor
// ============================================================================
template <typename T, size_t N>
IcArrayQueueTemp<T,N> :: IcArrayQueueTemp ( )
{
    m_priv = new IcArrayQueueTemp::IcArrayQueueTempPriv;
}

// ============================================================================
// dtor
// ============================================================================
template <typename T, size_t N>
IcArrayQueueTemp<T,N> :: ~IcArrayQueueTemp( )
{   delete m_priv; }

// ============================================================================
// clear all elements
// ============================================================================
template <typename T, size_t N>
void  IcArrayQueueTemp<T,N> :: clear()
{
    while ( ! m_priv->m_queue.isEmpty() ) {
        typename IcNodeQueueTemp<T*>::Node *n = m_priv->m_queue.dequeue();
        if ( n != nullptr ) {
            m_priv->m_free_list.takeAndEnqueue( n );
        }
    }
}

// ============================================================================
// enqueue an object
// ============================================================================
template <typename T, size_t N>
bool  IcArrayQueueTemp<T,N> :: enqueue( const T &d )
{
    typename IcNodeQueueTemp<T*>::Node *n = m_priv->m_free_list.dequeue();
    if ( n != nullptr ) {
        *( n->data() ) = d; // assign copy...
        m_priv->m_queue.takeAndEnqueue( n );
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// dequeue an object
// ============================================================================
template <typename T, size_t N>
bool  IcArrayQueueTemp<T,N> :: dequeue( T &d )
{
    typename IcNodeQueueTemp<T*>::Node *n = m_priv->m_queue.dequeue();
    if ( n != nullptr ) {
        d = *( n->data());                       // assign copy....
        *( n->data() ) = m_priv->m_slot[ N ];    // set to default
        m_priv->m_free_list.takeAndEnqueue( n ); // put it back to free list
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// force enqueue an object
// ============================================================================
template <typename T, size_t N>
void  IcArrayQueueTemp<T,N> :: forceEnqueue (
    const T &d, void (* dropObj )( T&, void* ), void *ctxt
) {
    IcLCG lcd;
    int cnt = static_cast<int>( lcd.value() % 100 ) + 50;

    while ( ! enqueue( d ) ) {
        // --------------------------------------------------------------------
        // random yeild thread if cost too many times
        // --------------------------------------------------------------------
        if ( -- cnt < 1 ) {
            std::this_thread::yield();
            cnt = static_cast<int>( lcd.value() % 100 ) + 50;
        }

        // --------------------------------------------------------------------
        // force dequeue a node, so we can get a free node
        // --------------------------------------------------------------------
        {
            T tmp;
            dequeue( tmp );
            if ( dropObj != nullptr ) { dropObj( tmp, ctxt ); }
        }
    }
}


// ////////////////////////////////////////////////////////////////////////////
//                    [ Thread Safe Class ]
/*!
 * @brief array type queue template by spec. size
 */
// ////////////////////////////////////////////////////////////////////////////
template <typename T>
class IcArrayQueuePsTemp {
private:
    class IcArrayQueuePsTempPriv {
    private:
        IcNodeQueueTemp<T*> m_free_list, m_queue;
        typename IcNodeQueueTemp<T*>::Node  *m_node;
        T  *m_slot; size_t  m_node_num;
    friend class IcArrayQueuePsTemp;

        // --------------------------------------------------------------------
        // ctor
        // --------------------------------------------------------------------
        IcArrayQueuePsTempPriv( size_t sz ) :
            m_free_list( nullptr, nullptr ), m_queue( nullptr, nullptr )
        {
            m_node_num = sz;
            m_node = new typename IcNodeQueueTemp<T*>::Node[ sz + 1 ];
            m_slot = new T [ sz + 1 ];
            for ( size_t i = 0; i < sz; i ++ ) {
                T* ptr = & m_slot[i];
                m_node[i].setData( ptr );
                m_free_list.takeAndEnqueue( & m_node[i] );
            }
        }

        // --------------------------------------------------------------------
        //  dtor
        // --------------------------------------------------------------------
        ~IcArrayQueuePsTempPriv ( )
        {
            m_free_list.clear();
            m_queue.clear();
            delete[] m_node;
            delete[] m_slot;
        }
    };

    IcArrayQueuePsTempPriv *m_priv;
public :
    //! ctor
    IcArrayQueuePsTemp ( size_t sz );

    //! dtor
    ~IcArrayQueuePsTemp( );

    //! check if is empty
    inline bool isEmpty( ) { return m_priv->m_queue.isEmpty(); }

    //! clear all objects
    //! @note call this function before should stop other threads to visite enqueue()...
    void  clear( );

    //! enqueue a object
    //! @param d [ in ] the data const ref.
    //! @return true means enqueued
    //! @note  parameter 'd' must implement 'operator ='
    bool  enqueue ( const T &d );

    //! dequeue a object
    //! @param d [out]  the data ref. output
    //! @return true means dequeued; otherwise means empty
    //! @note parameter 'd' must implement 'operator ='
    bool  dequeue ( T &d );

    //! force enqueue the object
    //! @param d        [in]      the data ref. input
    //! @param dropNode [opt_in]  the callback used to handle the dropped Node
    //! @param ctxt     [opt_in]  dropNode callback context pointer
    //! @note this function will call dequeue() to free node if null..
    void  forceEnqueue ( const T &d, void (* dropObj )( T&, void* ) = nullptr, void *ctxt = nullptr );

    QXPACK_IC_DISABLE_COPY( IcArrayQueuePsTemp )
};

// ============================================================================
// ctor
// ============================================================================
template <typename T>
IcArrayQueuePsTemp<T> :: IcArrayQueuePsTemp ( size_t sz )
{
    m_priv = new IcArrayQueuePsTemp::IcArrayQueuePsTempPriv( sz );
}

// ============================================================================
// dtor
// ============================================================================
template <typename T>
IcArrayQueuePsTemp<T> :: ~IcArrayQueuePsTemp( )
{   delete m_priv; }

// ============================================================================
// clear all elements
// ============================================================================
template <typename T>
void  IcArrayQueuePsTemp<T> :: clear()
{
    while ( ! m_priv->m_queue.isEmpty() ) {
        typename IcNodeQueueTemp<T*>::Node *n = m_priv->m_queue.dequeue();
        if ( n != nullptr ) {
            m_priv->m_free_list.takeAndEnqueue( n );
        }
    }
}

// ============================================================================
// enqueue an object
// ============================================================================
template <typename T>
bool  IcArrayQueuePsTemp<T> :: enqueue( const T &d )
{
    typename IcNodeQueueTemp<T*>::Node *n = m_priv->m_free_list.dequeue();
    if ( n != nullptr ) {
        *( n->data() ) = d; // assign copy...
        m_priv->m_queue.takeAndEnqueue( n );
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// dequeue an object
// ============================================================================
template <typename T>
bool  IcArrayQueuePsTemp<T> :: dequeue( T &d )
{
    typename IcNodeQueueTemp<T*>::Node *n = m_priv->m_queue.dequeue();
    if ( n != nullptr ) {
        d = *( n->data());                       // assign copy....
        *( n->data() ) = m_priv->m_slot[ m_priv->m_node_num ];  // set to default
        m_priv->m_free_list.takeAndEnqueue( n ); // put it back to free list
        return true;
    } else {
        return false;
    }
}

// ============================================================================
// force enqueue an object
// ============================================================================
template <typename T>
void  IcArrayQueuePsTemp<T> :: forceEnqueue (
    const T &d, void (* dropObj )( T&, void* ), void *ctxt
) {
    IcLCG lcd;
    int cnt = static_cast<int>( lcd.value() % 100 ) + 50;

    while ( ! enqueue( d ) ) {
        // --------------------------------------------------------------------
        // random yeild thread if cost too many times
        // --------------------------------------------------------------------
        if ( -- cnt < 1 ) {
            std::this_thread::yield();
            cnt = static_cast<int>( lcd.value() % 100 ) + 50;
        }

        // --------------------------------------------------------------------
        // force dequeue a node, so we can get a free node
        // --------------------------------------------------------------------
        {
            T tmp;
            dequeue( tmp );
            if ( dropObj != nullptr ) { dropObj( tmp, ctxt ); }
        }
    }
}

}
#endif
