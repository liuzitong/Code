// /////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_stdioipc
  @author  night wing
  @date    2018/09
  @brief   read write though stdio
  @version 0.1.0.0
*/
// /////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_STDIOIPC_HXX
#define QXPACK_IC_STDIOIPC_HXX

/*!
  @addtogroup  QxPack
  @{
*/
/*!
  @addtogroup  indcom
  @{
*/
/*!
  @addtogroup  sys_Exported_Classes
  @{
*/

#include <qxpack/indcom/common/qxpack_ic_def.h>
#include <qxpack/indcom/common/qxpack_ic_bytearray.hxx>

namespace QxPack {

// ////////////////////////////////////////////////////////
/*!
 * @brief stdio ipc accessor
 * @note  this class can used in non-QT5 program
 */
// ////////////////////////////////////////////////////////
class QXPACK_IC_API  IcStdioIpc {
public:
    //! ctor
    IcStdioIpc( );

    //! dtor
    virtual ~IcStdioIpc( );

    /*!
     * @brief read a record
     * @param [out] is_ok, this pointer will output result of read
     * @return if read successfully, return a valid object.
     */
    IcByteArray  readRec ( bool *is_ok = nullptr );

    /*!
     * @brief write a record
     * @param [in] data, the source data
     * @param [in] data_size, the source data size count in bytes
     * @return true for wrote.
     */
    bool         writeRec( const char *data, int data_size );

    /*!
     * @brief  read all data from stdin
     * @param [in] block_size, temporary block size
     * @param [in] is_ok, a result pointer
     * @return if successfully, return valid object
     */
    IcByteArray  readAll ( int block_size, bool *is_ok = nullptr );

    /*!
     * @brief write data to stdout
     * @param [in] data, the source data
     * @param [in] data_size, the source data size count in bytes
     * @return true for wrote.
     */
    bool         write   ( const char *data, int data_size );

    /*!
     * @return true for the stdin has been closed by caller program.
     */
    static bool  isStdInClosed( );

protected:
    IcStdioIpc( const IcStdioIpc & );
    IcStdioIpc &  operator = ( const IcStdioIpc & );
private:
    void *m_obj;
};

}

#if defined( QXPACK_IC_QT_ENABLED )

#include <QObject>
#include <QProcess>

namespace QxPack {

// ///////////////////////////////////////////////////////
/*!
 * @brief QT Process used StdIO read write accessor
 */
// //////////////////////////////////////////////////////
class QXPACK_IC_API IcQProcStdioIpc : public QObject {
    Q_OBJECT
public:
    /*!
     * @brief constructor
     * @param [in] proc, the QProcess object
     */
    IcQProcStdioIpc( QProcess &proc );

    virtual ~IcQProcStdioIpc( );

    /*!
     * @brief read record from target program stdout
     * @param [in] is_ok, if read ok, output true
     * @return if read successfully, return valid object
     */
    IcByteArray  readRec ( bool *is_ok );

    /*!
     * @brief write record to target program stdin
     * @param [in] data, source data
     * @param [in] data_size, source data size count in bytes
     * @return true for wrote
     */
    bool         writeRec( const char *data, int data_size  );

    /*!
     * @brief read all data from target program stdout
     * @param [in] block_size, temporary block used by read
     * @param [out] is_ok, output true if read ok
     * @return if read successfully, return valid object
     */
    IcByteArray  read ( int block_size, bool *is_ok );

    /*!
     * @brief write data to target program
     * @param [in] data, the source data
     * @param [in] data_size, the source data size count in bytes
     * @param [in] close_after_write, if true, QProcess will close the pipe.
     * @return true for wrote
     */
    bool         write( const char *data, int data_size, bool close_after_write = false );

protected:
    Q_SLOT  void  qprocess_onDestroyed( QObject* );
private:
    void *m_obj;
    Q_DISABLE_COPY( IcQProcStdioIpc )
};

}
#endif

/*!
  @}
*/
/*!
  @}
*/
/*!
  @}
*/

#endif
