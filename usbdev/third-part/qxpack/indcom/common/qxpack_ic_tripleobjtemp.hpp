/* ///////////////////////////////////////////////////////////////////// */
/*! 
  @file    qxpack_ic_tripleobjtemp.hpp
  @author  night wing
  @date    2018/11
  @brief   The file declare the triple object Template
  @par     History
  @verbatim
          <author>   <time>   <version>  <desc>                                   
         nightwing   2018/11   0.1.0     design
  @endverbatim
*/
/* ///////////////////////////////////////////////////////////////////// */
#ifndef  QXPACK_IC_TRIPLEOBJTEMP_HPP
#define  QXPACK_IC_TRIPLEOBJTEMP_HPP


#include <stdint.h>
#include <atomic>    // C++11
#include <cstdlib>
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"

namespace QxPack {

// /////////////////////////////////////////////////////////////////////
// private objects
// /////////////////////////////////////////////////////////////////////
template <typename T>
class  IcTripleObjTempPriv : public IcPImplPrivTemp< IcTripleObjTempPriv<T> >
{
private:
    std::atomic<int>  m_tmp_idx;
    int  m_in_idx, m_out_idx;
    T    m_data[3];
public :
    IcTripleObjTempPriv ( )
    { m_in_idx = 0; m_out_idx = 2; m_tmp_idx.store(1); }

    IcTripleObjTempPriv ( const IcTripleObjTempPriv & )
    { std::abort( ); }

    virtual ~IcTripleObjTempPriv ( ) { }

    void  swap( bool is_data_on, bool *is_old_data_picked = nullptr );

    inline T*  writable ( ) { return &m_data[ m_in_idx ];  }
           T*  readable ( );
    inline T*  lastReadable ( ) { return &m_data[ m_out_idx ]; }

    inline bool isDataOn( ) { return ( m_tmp_idx.load( ) & 0x80 ? true : false ); }
};

// =======================================================
//  return a readable object
// =======================================================
template <typename T >
T*   IcTripleObjTempPriv<T> :: readable( )
{
    int tmp_idx;

    if ( (( tmp_idx = m_tmp_idx.load()) & 0x80 ) != 0 ) { // existed data
        do {
          if ( m_tmp_idx.compare_exchange_strong( tmp_idx, m_out_idx )) {
              m_out_idx = ( tmp_idx & 0x7f ); if ( m_out_idx > 2 ) { m_out_idx = 0; }
              break;
          }
        } while ( true );
        return & m_data[ m_out_idx ];
    } else {
        return nullptr;
    }
}

// ========================================================
// swap the data
// ========================================================
template <typename T>
void  IcTripleObjTempPriv<T> :: swap ( bool is_data_on, bool *is_old_data_picked )
{
    int tmp_idx; int flag = ( is_data_on ? 0x80 : 0x00 );

    do {
        tmp_idx = m_tmp_idx.load( );
        if ( m_tmp_idx.compare_exchange_strong( tmp_idx, m_in_idx | flag )) {
            m_in_idx = tmp_idx & 0x7f;
            break;
        }
    } while ( true );

    if ( is_old_data_picked != nullptr ) { *is_old_data_picked = ( tmp_idx & 0x80 ? false : true ); }
}

// //////////////////////////////////////////////////////////////
//
// wrapper API
//
// //////////////////////////////////////////////////////////////
template <typename T>
class  IcTripleObjTemp {
private: void *m_obj;
public :
    IcTripleObjTemp ( )
    { m_obj = nullptr; IcTripleObjTempPriv<T>::buildIfNull( &m_obj ); }

    IcTripleObjTemp ( const IcTripleObjTemp<T> &other )
    { m_obj = nullptr; IcTripleObjTempPriv<T>::attach( & m_obj, const_cast< void **>( & other.m_obj )); }

    IcTripleObjTemp &  operator = ( const IcTripleObjTemp<T> &other )
    { IcTripleObjTempPriv<T>::attach( & m_obj, const_cast<void **>( & other.m_obj )); return *this; }

    virtual ~IcTripleObjTemp( )
    {
        if ( m_obj != nullptr ) {
            IcTripleObjTempPriv<T>::attach( & m_obj, nullptr );
        }
    }

    inline T* writable()     { return static_cast< IcTripleObjTempPriv<T> *>( m_obj )->writable(); }
    inline T* lastReadable() { return static_cast< IcTripleObjTempPriv<T> *>( m_obj )->lastReadable(); }
    inline T* readable()     { return static_cast< IcTripleObjTempPriv<T> *>( m_obj )->readable(); }
    inline void swap( bool is_data_on, bool *last_data_picked = nullptr )
    { static_cast< IcTripleObjTempPriv<T> *>( m_obj )->swap( is_data_on, last_data_picked ); }
    inline bool isDataOn()   {  }
};




}


#endif
