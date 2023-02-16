#ifndef QXPACK_IC_NUMSTATAFQTEMP_HPP
#define QXPACK_IC_NUMSTATAFQTEMP_HPP

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <stdint.h>
#include <cassert>
#include <cstdlib>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  numeric statistics queue ( always full queue )
 */
// ////////////////////////////////////////////////////////////////////////////
template <typename T, size_t N >
class  IcNumStatQueueTemp {
private:
    T  m_elem[ N ]; T  m_max;
    size_t m_cur, m_size;
public :
    explicit IcNumStatQueueTemp( );
    virtual ~IcNumStatQueueTemp( );

    //! check it is empty
    inline bool    isEmpty( ) const { return m_size == 0; }

    //! check it is already full
    inline bool    isFull ( ) const { return m_size == N; }

    //! return the current element number
    inline size_t  size    ( ) { return m_size; }

    //! return the capacity of the queue
    inline size_t  capacity( ) { return N;      }

    //! return current maxinum element
    //! @note if queue is empty, return value is undefined.
    inline T       maxElem ( ) { return m_max;  }

    //! enqueue and dequeue
    //! @return if empty, return the default T value
           T       enqAndDeq ( const T&);
};

// ============================================================================
// ctor
// ============================================================================
template <typename T, size_t N>
IcNumStatQueueTemp<T,N> :: IcNumStatQueueTemp ( )
  : m_cur(0), m_size(0)
{ }

// ============================================================================
// dtor
// ============================================================================
template <typename T, size_t N>
IcNumStatQueueTemp<T,N> :: ~IcNumStatQueueTemp( )
{ }

// ============================================================================
/*!
 * @brief enqueue a number, and pop the oldest value
 * @note if the queue is not full, always return the first element. \n
 * use @see IcNumStatQueueTemp::size() to ensure the current element number in the queue.
 */
// ============================================================================
template <typename T, size_t N>
T   IcNumStatQueueTemp<T,N> :: enqAndDeq ( const T &e )
{
    T  oldest;

    // push the element, pop oldest
    if ( m_size < N ) {
        m_elem[ m_cur ++ ] = e;
        oldest = m_elem[ 0 ];
        ++ m_size;
    } else {
        oldest = m_elem[ m_cur ];
        m_elem[ m_cur ++ ] = e;
    }
    if ( m_cur >= N ) { m_cur = 0; }

    // update the maximum number
    if ( m_size == 1 ) {
        m_max = e;
    } else {
        if ( m_max < e ) { m_max = e; }
    }

    return oldest;
}




}

#endif
