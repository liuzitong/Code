#ifndef QXPACK_IC_DATATRANS_BUFF_HXX
#define QXPACK_IC_DATATRANS_BUFF_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QIODevice>
#include <QByteArray>
#include <QSharedPointer>
#include <QVariant>
#include <QVector>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief the factory function rules
 * @param ctxt [in] the context pointer
 * @param opr  [in] a string operation
 * @param var  [in] optional var.
 * @return by opr type. see below example
 * @example
 *  1) called with: ret = factor( ctxt, "create_reader", var );  \n
 *     the var is QSharedPointer<QIODevice>, output the reader wrapped with QVariant.  \n
 *     ret is QVariant::fromValue( QObject* ). \n
 *
 *  2) called with: ret = factor( ctxt, "create_writer", var );  \n
 *     the var is QSharedPointer<QIODevice>, output the writer  wrapped with QVariant. \n
 *     ret is QVariant::fromValue( QObject* ) \n
 */
// ////////////////////////////////////////////////////////////////////////////
#define STR_IcDataTransFactory_createReader QStringLiteral("create_reader")
#define STR_IcDataTransFactory_createWriter QStringLiteral("create_writer")
typedef QVariant  (* IcDataTransFactory )( void *ctxt, const QString &opr, const QVariant &var );

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief provide a buffer to splicing raw data package
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcDataTransPkgReadBuff : public QObject {
    Q_OBJECT
public:
    //! the analysis information
    //! @details the AnalyInfo is used for describe the analysis result of the data stream. \n
    //! the 'section' means a range of a valid package. the data stream should be combined \n
    //! by multiple sections. e.g [ section 0 ][ section 1 ][ section 2 ]...
    struct AnalyInfo {
        int  m_cb_size;          //! this structure size
        int  m_sect_size;        //! section size in data
        int  m_sect_ofv;         //! section offset in data
        int  m_data_ofv_in_sect; //! means the data offset in the section
        int  m_data_size;        //! avaliable data size
    };

    //! ctor
    //! @param io_dev [in] the io device shared pointer
    //! @note  'io_dev' only used QIODevice::bytesAvailable(), QIODevice::read(),  QIODevice::readyRead()
    explicit IcDataTransPkgReadBuff ( QSharedPointer<QIODevice> &io_dev, QObject *pa = Q_NULLPTR );

    //! ctor 2 ( custom the maximum read buffer )??

    //! dtor
    virtual ~IcDataTransPkgReadBuff ( ) Q_DECL_OVERRIDE;

    //! clear
    Q_SLOT  virtual void  clear( );

    //! read the data
    Q_SLOT  virtual void  readData( );

    /*!
     * @brief analysis data of current data
     * @param ba  [in]  the data stream object
     * @param ai  [out] the offset the data
     * @return true means found data, otherwise means no data or data is not completed
     * @note  user can override it as custom analysis
     */
    virtual  bool    analyData ( const QByteArray &ba, AnalyInfo &ai );

    //! check if need do filter the read data, default return false
    virtual  bool    isReqFilterData( ) const;

    //! do the data filter
    //! @param ba    [in] the current hole data
    //! @param rd_ba [in] current read data
    //! @return true means can append the rd_ba
    //! @cond  if isReqFilterData( ) return true, this function can work, otherwise ignore this function
    virtual  bool    filterData( const QByteArray &ba, QByteArray &rd_ba );

    //! get the default maximum reader concatenate buffer size, normally return 16MB
    //! nw: 2019/12/31 added.
    //! @note  While analy data failed, the inner buffer will be increased if remote client sends \n
    //!  more and more data. This function return the maximum size of the inner buffer.
    virtual  int     maxCatBuffSize( ) const;

    //! a signal that means got a new package
    //! @warning if user want to limit the recv. data package, should \n
    //!  DO IT on the upper layer!
    Q_SIGNAL void    newPackage( const QByteArray & );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcDataTransPkgReadBuff )
};




// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief provide a buffer to do send package
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcDataTransPkgWriteBuff : public QObject {
    Q_OBJECT
public:
    //! ctor
    //! @param io_dev [in] shared QIODevice pointer
    //! @note 'io_dev' only used QIODevice::bytesWritten(), QIODevice::write()
    explicit IcDataTransPkgWriteBuff ( QSharedPointer<QIODevice> &io_dev, QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcDataTransPkgWriteBuff ( ) Q_DECL_OVERRIDE;

    //! clear content
    Q_SLOT  virtual void    clear( );

    //! do the post data
    //! @param ba [in] orignal package.
    //! @param is_pkg [in] is package .. true while the package sent, it will emit packageSent()
    //! @return true means post.
    Q_SLOT  virtual bool    post( const QByteArray &ba, bool is_pkg = true );

    /*!
     * @brief pack the data as transfer package
     * @param dat      [ in ] the data package
     * @param ba_list  [ out] packed result package list
     * @return return false means use default header
     * @note user can override it
     * @note 'ba_vect' item QPair<QByteArray,bool> is a pair ( data, is data flag )
     */
    Q_SLOT  virtual bool  packData ( const QByteArray &dat, QVector<QPair<bool,QByteArray>> &ba_list );

    //! signal about a package sent
    Q_SIGNAL void   packageSent( );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcDataTransPkgWriteBuff )
};


}

#endif
