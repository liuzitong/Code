// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_bytearray
  @author  night wing
  @date    2018/09
  @brief   bytearray object, used for store string or binary data
  @version 0.1.1.0
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_BYTEARRAY_HXX
#define QXPACK_IC_BYTEARRAY_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief IcByteArray
 * @details \n
 *   a) normal buffer \n
 *     the buffer is managed by private object.\n
 *     ctor( copy ), assign( copy ) -- point to same private object.\n
 *     write( copy )   -- do a deep copy. \n
 *     resize, reserve -- adjust memory. \n
 *     \n
 *   b) point to existed const data \n
 *     isConst() return true. isRefData() return true. \n
 *     ctor( copy ), assign( copy ) -- point to same private object. \n
 *     write( copy )   -- do a deep copy.\n
 *     resize, reserve -- directly do deep copy to a adjusted memory.\n
 *
 *     \n
 *   c) point to existed data \n
 *      same as b) but write() do not perform a deep copy. \n
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcByteArray {
public:
    typedef void (* CleanUpFunction )( void* );

    /*!
     * @brief construct a null object
     */
    IcByteArray( );

    /*!
     * @brief construct a specified size object
     * @param [in] required size
     * @note  the object will allocate 'size' space for store content.
     */
    IcByteArray( int size );

    /*!
     * @brief construct by other object
     * @note  this object is shared data with other one, unless \n
     *        this or other call data() or other modified functions.
     */
    IcByteArray( const IcByteArray & );

    /*!
     * @brief  assigne copy other object
     * @return object reference
     * @note   same as CTOR copy, this object is shared data with other one.
     */
    IcByteArray &  operator = ( const IcByteArray & );

    /*!
     * @note DTOR only detached from the shared data, if the shared data is zero,\n
     *       it will be free.
     */
    ~IcByteArray( );

    /*!
     * @brief resize current space, change inner space to 'size'
     * @param [in] size, extend or shrink space into this size
     * @warning if the IcByteArray point to a const string, call resize will do follow operation: \n
     *
     */
    void    resize ( int size );

    /*!
     * @brief reserve at least 'size' space
     * @param [in] size, extend space to size, or do nothing
     * @note if current space bigger than 'size', nothing happen
     */
    void    reserve( int size );

    /*!
     * @brief  check if current object is empty or not.
     * @return true means no data in the object
     */
    bool    isEmpty( ) const;

    //! check if current data is const
    //! @return true or false. for empty object, return true.
    bool    isConst( ) const;

    //! check if current data is referenced data
    //! @return true or false. for empty object, return false.
    bool    isRefData() const;

    /*!
     * @return return the data size in the object
     */
    int     size( ) const;

    /*!
     * @return return inner data pointer
     * @warning this operation will cause a deep copy if data shared with other \n
     *    except isConst() is false and isRefData() is true
      */
    char *  data( );

    /*!
     * @return return inner data pointer ( constant )
     */
    const char * constData( ) const;

    // the user data access API
    int  userDataSize ( ) const;
    int  writeUserData( const char* sp, int size );
    int  readUserData (       char* dp, int size );

    /*!
     * @brief construct a copy of user data
     * @param [in] point to the raw data
     * @param [in] the raw data length in bytes, -1 means the raw data \n
     *        need to be test.
     * @note  the user data will be copied
     */
    IcByteArray( const char *data, int len = -1 );

    //! do a copy operation
    IcByteArray  copy() const;

    /*!
     * @brief  override the operator <
     * @return true means this object is less than other
     */
    bool operator < ( const IcByteArray & ) const;

    /*!
     * @brief   override the operator ==
     * @return  true means this object content is equal to other.
     */
    bool operator ==( const IcByteArray & );

    //! move ptr from other object
    inline void   movePtrFrom( IcByteArray &ot )
    { if ( m_obj != nullptr ){ makeNull(); } m_obj = ot.m_obj; ot.m_obj = nullptr; }

    /*!
     * @brief   construct a IcByteArray point to static const data.
     * @details only point to the const data, do not allocate memory to copy data
     */
    static IcByteArray  fromConstData( const char     *data, int e_num = -1 );

    //! override
    static IcByteArray  fromConstData( const uint16_t *data, int num = -1  );

    /*!
     * @brief keep a reference to user data
     * @param [in] data, the const data pointer
     * @param [in] len,  the size of user data, can be -1 if it is a ascii-z string
     * @param [in] cl_func, the cleanup function.
     * @param [in] cl_obj,  the cleanup info
     * @note  any write operation will cause a deep copy. data() function will perform \n
     *      a deep copy. Normally, isConst() return true, isRefData() return true.
     * @warning  The user data must remain valid throughout the life of the IcByteArray \n
     *      and all copies that have not been modified or otherwise detached from the \n
     *      original buffer
     */
    static IcByteArray  fromConstData ( const char     *data, int len, CleanUpFunction cl_func, void *cl_obj );

    /*!
     * @brief keep a reference to user data
     * @param [in] data, the data pointer
     * @param [in] len,  the size of user data, can be -1 if it is a ascii-z string
     * @param [in] cl_func, the cleanup function.
     * @param [in] cl_obj,  the cleanup info
     * @note  this ctor will do a singleton reference data. Any write operation and data() \n
     *    do not perform a deep copy. Normally, isConst() return false, isRefData() return true.
     */
    static IcByteArray  fromData ( char *data, int len,  CleanUpFunction cl_func, void *cl_obj );

    //! free the cached IcByteArray object
    static void  cleanupCache( );


protected:
    //! @brief make this object is null
    void  makeNull();

private:
    void *m_obj;
};

}

#endif
