// ////////////////////////////////////////////////////////////////////////////
/*!
 * @file qxpack_ic_datatrans_buff
 * @author nightwing
 * @date 2019/06
 * @verbatim
 *     <author>    <date>    <history memo.>
 *     nightwing  2019/03     build
 *     nightwing  2019/05     fixed.
 *     nightwing  2019/05     check point
 *     nightwing  2019/06     fixed
 *     nightwing  2019/12     fixe the package checking error..
 * @endverbatim
 */
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_DATATRANS_BUFF_CXX
#define QXPACK_IC_DATATRANS_BUFF_CXX

#include "qxpack_ic_datatrans_buff.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_numstatqueuetemp.hpp"

#include <QList>
#include <QString>
#include <QTimer>
#include <QByteArray>
#include <QtGlobal>
#include <QObject>
#include <new>
#include <QDebug>
#include <QPair>
#include <cstdlib>
#include <cstring>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// 
//                   package data header ( default implement )
//
// ////////////////////////////////////////////////////////////////////////////
//! default trans package header
struct IcDataTransPkg_Header {
    quint64  m_data_size;   // data size ( value order by LE or BE )
    quint16  m_magic;       // always assign 0x55aa, so LE should be 0xaa, 0x55 bytes
    quint16  m_version;     // always assign 0x0001, so LE should be 0x01, 0x00
    quint16  m_hdr_chksum;  // check sum value, expect self. NOTE the system byte byte order
    quint16  m_rsvd;        // rsvd; always 0
};

class QXPACK_IC_HIDDEN  IcDataTransPkg_HeaderWrap {
private  : IcDataTransPkg_Header m_hdr;
protected:
    quint16  checkSumHeader ( )
    {
        uint8_t *dp = reinterpret_cast<uint8_t*>( & m_hdr ); quint16 sum = 0;
        for ( size_t x = 0; x < sizeof( m_hdr ) - sizeof(quint16) * 2; x ++ ) {
            sum += *dp ++;
        }
        return ~sum;
    }

public :
    // ------------------------------------------------------------------------
    // ctor with parameter
    // ------------------------------------------------------------------------
    IcDataTransPkg_HeaderWrap ( const char *, quint64 ds )
    {
        m_hdr.m_data_size  = ds;
        m_hdr.m_magic      = 0x55aa;
        m_hdr.m_version    = 0x0001;
        m_hdr.m_rsvd       = 0;
        m_hdr.m_hdr_chksum = this->checkSumHeader();
    }

    // ------------------------------------------------------------------------
    // ctor 
    // ------------------------------------------------------------------------
    explicit IcDataTransPkg_HeaderWrap ( ) { }

    inline  void*  headerAddr( ) { return reinterpret_cast<void*>( & m_hdr ); }

    // ------------------------------------------------------------------------
    // check if header is LE or BE
    // ------------------------------------------------------------------------
    inline bool  isLE( )
    {
        uint8_t *mg = reinterpret_cast<uint8_t*>( & m_hdr.m_magic );
        return ( mg[0] == 0xaa  &&  mg[1] == 0x55 );
    }

    // ------------------------------------------------------------------------
    // verify the header check sum is OK or not
    // ------------------------------------------------------------------------
    inline bool  verify ( )
    {
        uint8_t *mg = reinterpret_cast<uint8_t*>( & m_hdr.m_magic );
        uint8_t *cs = reinterpret_cast<uint8_t*>( & m_hdr.m_hdr_chksum );
        if        ( mg[0] == 0xaa  &&  mg[1] == 0x55 ) { // target is LE
            uint16_t chk_sum = uint16_t( cs[0] | ( cs[1] << 8 ));
            return ( chk_sum == this->checkSumHeader());

        } else if ( mg[0] == 0x55  &&  mg[1] == 0xaa ) { // target is BE
            uint16_t chk_sum = uint16_t(( cs[0] << 8 ) | cs[1]);
            return ( chk_sum == this->checkSumHeader());

        } else {
            return false;
        }
    }

    // ------------------------------------------------------------------------
    // return the current data size
    // ------------------------------------------------------------------------
    inline quint64   dataSize( )
    {
        quint64 data_sz = 0;

        if ( this->isLE() ) {
            quint8 *ds = reinterpret_cast<quint8*>( & m_hdr.m_data_size ) + 7;
            for ( int x = 0; x < 8; x ++ ) {
                data_sz = ( data_sz << 8 ) | (*ds -- );
            }
        } else {
            quint8 *ds = reinterpret_cast<quint8*>( & m_hdr.m_data_size );
            for ( int x = 0; x < 8; x ++ ) {
                data_sz = ( data_sz << 8 ) | (*ds ++ );
            }
        }
        return data_sz;
    }
};

// ////////////////////////////////////////////////////////////////////////////
//
//                        Read buffer
//
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//  private object
// ////////////////////////////////////////////////////////////////////////////
#define T_ReadPriv( o )  T_ObjCast( IcDataTransPkgReadBuffPriv*, o )
class QXPACK_IC_HIDDEN IcDataTransPkgReadBuffPriv : public QObject {
    Q_OBJECT
private:
    qint64 m_wr_pos;  QByteArray m_buff;
    QSharedPointer<QIODevice>  m_io_dev_sp;
    IcDataTransPkgReadBuff    *m_parent;
    IcNumStatQueueTemp<qint64,8>  m_pkg_sz_queue;
    QTimer  m_shrink_chk_tmr;
protected:
    void  extendMem( qint64 rd_sz );
    Q_SLOT  void  shrinkMem( );

    #define RECV_BUFF_MINI_SIZE 4096
    static qint64 roundBlockSize( qint64 sz )
    { return (( sz + RECV_BUFF_MINI_SIZE - 1 ) / RECV_BUFF_MINI_SIZE ) * RECV_BUFF_MINI_SIZE; }

public :
    explicit IcDataTransPkgReadBuffPriv ( IcDataTransPkgReadBuff *, QSharedPointer<QIODevice> & );
    virtual ~IcDataTransPkgReadBuffPriv ( ) Q_DECL_OVERRIDE;
    inline void   clear()             { m_wr_pos = 0; m_buff = QByteArray(); }
    inline void   resetWrPos( int p ) { m_wr_pos = ( p >= m_buff.size() ? m_buff.size() : ( p > 0 ? p : 0 )); }

    bool   appendData  ( );
    bool   dumpPackage ( IcDataTransPkgReadBuff::AnalyInfo & );
    inline QByteArray &  dataBuff( ) { return m_buff; }
};

// ============================================================================
// ctor
// ============================================================================
IcDataTransPkgReadBuffPriv :: IcDataTransPkgReadBuffPriv(
    IcDataTransPkgReadBuff *pa, QSharedPointer<QIODevice> &dev
) : m_wr_pos( 0 )
{
    m_parent = pa;

    // ------------------------------------------------------------------------
    // handle the io device read signal
    // ------------------------------------------------------------------------
    if ( dev.data() == Q_NULLPTR ) { qFatal("The QIODevice pointer is null."); }
    m_io_dev_sp = dev;
    QObject::connect (
        m_io_dev_sp.data(), SIGNAL(readyRead()), m_parent, SLOT(readData()), Qt::QueuedConnection
    );

    // ------------------------------------------------------------------------
    // prepare a memory shrink timer
    // ------------------------------------------------------------------------
    QObject::connect( & m_shrink_chk_tmr,SIGNAL(timeout()), this, SLOT(shrinkMem()) );
    m_shrink_chk_tmr.setInterval(3000);
    m_shrink_chk_tmr.setSingleShot(false);
    m_shrink_chk_tmr.start();
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransPkgReadBuffPriv :: ~IcDataTransPkgReadBuffPriv ( )
{
    // need not to care any signals of the io device
    QObject::disconnect( m_io_dev_sp.data(), Q_NULLPTR, m_parent, Q_NULLPTR );

    // stop memory shrink timer
    m_shrink_chk_tmr.stop();
    QObject::disconnect( &m_shrink_chk_tmr, Q_NULLPTR, this, Q_NULLPTR );
}

// ============================================================================
// extend the memory, so we can safely read out the data
// ============================================================================
void  IcDataTransPkgReadBuffPriv :: extendMem( qint64 total_cnt )
{
    qint64 req_cnt = m_wr_pos + total_cnt;
    if ( qint64( m_buff.size() ) < req_cnt ) {
        qint64 round_sz = IcDataTransPkgReadBuffPriv::roundBlockSize( req_cnt * 2 );
        qDebug() << "extend mem from " << m_buff.size() << " to " << int( round_sz );
        m_buff.resize( int( round_sz ) );
    }
    if ( qint64( m_buff.size()) < req_cnt ) {
        qFatal("Can not allocate enough memory for read data");
    }
}

// ============================================================================
// free the memory or shrink the memory
// ============================================================================
void  IcDataTransPkgReadBuffPriv :: shrinkMem()
{
    if ( m_buff.isEmpty() ) { return; }

    qint64 round_wr_sz = IcDataTransPkgReadBuffPriv::roundBlockSize( m_wr_pos );
    qint64 max_pkg_sz  = ( ! m_pkg_sz_queue.isEmpty() ? m_pkg_sz_queue.maxElem() : RECV_BUFF_MINI_SIZE );
    qint64 estm_sz = qMax( round_wr_sz, max_pkg_sz ) * 2 + RECV_BUFF_MINI_SIZE;
    qint64 buff_sz = m_buff.size();

    if ( estm_sz < buff_sz && buff_sz > RECV_BUFF_MINI_SIZE ) {
        qint64 tgt_sz = IcDataTransPkgReadBuffPriv::roundBlockSize( buff_sz / 2 );
        m_buff.resize( int( tgt_sz ));
        qDebug() << "shrink mem from " << buff_sz  << " to " << int( tgt_sz );
    }
}

// ============================================================================
// read the data from IO Device
// ============================================================================
bool  IcDataTransPkgReadBuffPriv :: appendData ( )
{
    // ------------------------------------------------------------------------
    // estimate the memory, so we can get enough memory
    // ------------------------------------------------------------------------
    if ( ! m_io_dev_sp->isReadable()) { return false; }

    qint64 total_cnt = m_io_dev_sp->bytesAvailable();
    if ( total_cnt < 1 ) { return false; }
    this->extendMem( total_cnt );

    // ------------------------------------------------------------------------
    // read data from device
    // ------------------------------------------------------------------------
    qint64 rd_cnt = m_io_dev_sp->read( m_buff.data() + m_wr_pos, total_cnt );
    if ( rd_cnt >= 0 ) { m_wr_pos += rd_cnt; } // rd_cnt( -1 ) means error.

    return ( rd_cnt >= 0 );
}

// ============================================================================
// try dump the package,
// ============================================================================
bool   IcDataTransPkgReadBuffPriv :: dumpPackage( IcDataTransPkgReadBuff::AnalyInfo &ai )
{
    // ------------------------------------------------------------------------
    // check the current analysis data
    // ------------------------------------------------------------------------
    if ( size_t( ai.m_cb_size ) < sizeof( IcDataTransPkgReadBuff::AnalyInfo))
    { return false; }

    if ( ai.m_sect_ofv < 0         ||  ai.m_sect_size < 0  ||
         ai.m_data_ofv_in_sect < 0 ||  ai.m_data_size < 0 )
    { return false; }  // data must be vaild

    if ( qint64( ai.m_sect_ofv + ai.m_sect_size ) > m_wr_pos ||  // bad section range
         ai.m_data_ofv_in_sect + ai.m_data_size > ai.m_sect_size // bad data range
    ){ return false; }

    // ------------------------------------------------------------------------
    // dump data, and queued a signal
    // ------------------------------------------------------------------------
    QByteArray ba = m_buff.mid( ai.m_sect_ofv + ai.m_data_ofv_in_sect, ai.m_data_size );
    QMetaObject::invokeMethod (
        m_parent, "newPackage", Qt::QueuedConnection, Q_ARG( const QByteArray&, ba )
    );
    m_pkg_sz_queue.enqAndDeq( ai.m_sect_size );

    // ------------------------------------------------------------------------
    // move memory to start position
    // NOTE: the section range is need dropped, Normally, we can drop all data before
    // this section ( contain this section )
    // ------------------------------------------------------------------------
    qint64 used_sz = ai.m_sect_ofv + ai.m_sect_size;
    char *dp = m_buff.data();
    char *sp = dp + static_cast<qintptr>( used_sz );  // the remain data start pointer
    qint64 rem_sz = m_wr_pos - used_sz;

    if ( rem_sz > 0 ) {
        std::memmove( dp, sp, size_t( rem_sz ));
        m_wr_pos = rem_sz;
    } else {
        m_wr_pos = 0;
    }

    return true;
}

// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcDataTransPkgReadBuff :: IcDataTransPkgReadBuff (
    QSharedPointer<QIODevice> &dev, QObject *pa
) : QObject( pa )
{
    m_obj = qxpack_ic_new( IcDataTransPkgReadBuffPriv, this, dev );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransPkgReadBuff :: ~IcDataTransPkgReadBuff ( )
{
    qxpack_ic_delete( m_obj, IcDataTransPkgReadBuffPriv );
}

// ============================================================================
// clear all contents
// ============================================================================
void   IcDataTransPkgReadBuff :: clear( ) { T_ReadPriv( m_obj )->clear(); }

// ============================================================================
// SLOT: read the content
// ============================================================================
void   IcDataTransPkgReadBuff :: readData ( )
{
    // ------------------------------------------------------------------------
    // while we got the data, we try to emit every package in the stream.
    // ( nw: 20190813-1032 fixed, provide by CJ )
    // ------------------------------------------------------------------------
    bool ret = T_ReadPriv( m_obj )->appendData( ); // first, we should read all data from IODevice
    while ( ret ) {
        IcDataTransPkgReadBuff::AnalyInfo ai; ai.m_cb_size = -1;
        if ( ( ret = this->analyData( T_ReadPriv( m_obj )->dataBuff(), ai )) ) {
            ret = T_ReadPriv( m_obj )->dumpPackage( ai );
        } else {
            int  lmt_sz = this->maxCatBuffSize();// -1 means no limit
            if ( lmt_sz < 0 || T_ReadPriv( m_obj )->dataBuff().size() < lmt_sz ) { continue; }  // need more data..

            // ----------------------------------------------------------------
            // ( nw: 20191228-1523 added )
            // bad data, we need search in data buffer to find a header ??
            // ----------------------------------------------------------------
            QByteArray &ba = T_ReadPriv( m_obj )->dataBuff();
            for ( int test_ofv = 8, ba_sz = ba.size() - 8; ba_sz > 0 && ! ret; test_ofv += 8, ba_sz -= 8 ) {

                // build tmp ref. byte array to analysis..
                QByteArray tmp = QByteArray::fromRawData( ba.constData() + test_ofv, ba_sz );
                if ( ( ret = this->analyData( tmp, ai )) ) {
                     ba = ba.mid( test_ofv, ba_sz );         // found header, now we got the correct offset.
                     T_ReadPriv( m_obj )->resetWrPos( ba_sz );
                }
            }
            if ( ! ret ) { T_ReadPriv( m_obj )->clear(); }   // found nothing, clear all data..
        }
    }
}

// ============================================================================
// analysis the data header is existed or not ( default implement )
// ============================================================================
bool   IcDataTransPkgReadBuff :: analyData (
    const QByteArray &ba, IcDataTransPkgReadBuff::AnalyInfo &ai
) {
    // ------------------------------------------------------------------------
    // in default, we added a header before the data.
    // ------------------------------------------------------------------------
    if ( size_t( ba.size()) < sizeof( IcDataTransPkg_HeaderWrap ) ) { ai.m_cb_size = -1; return false; }
    IcDataTransPkg_HeaderWrap  hdr;
    std::memcpy( hdr.headerAddr(), ba.constData(), sizeof( hdr ) );
    if ( hdr.verify() ) {
        ai.m_cb_size   = sizeof( IcDataTransPkgReadBuff::AnalyInfo );
        ai.m_sect_ofv  = 0;
        ai.m_sect_size = int( sizeof( hdr ) + hdr.dataSize() );
        ai.m_data_ofv_in_sect = sizeof( hdr );
        ai.m_data_size = int( hdr.dataSize());
        return true;
    } else {
        ai.m_cb_size = 0; // bad header
        return false;
    }
}

// ============================================================================
// check if req filter the read data
// ============================================================================
bool    IcDataTransPkgReadBuff :: isReqFilterData( ) const { return false; }

// ============================================================================
// filter the data
// ============================================================================
bool    IcDataTransPkgReadBuff :: filterData( const QByteArray &, QByteArray & )
{ return false; }

// ============================================================================
// return the data buffer limit size
// ============================================================================
int     IcDataTransPkgReadBuff :: maxCatBuffSize() const
{ return 16 * 1024 * 1024; }


// ////////////////////////////////////////////////////////////////////////////
//
//                     Send Buffer
//
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// private object
// ////////////////////////////////////////////////////////////////////////////
typedef QPair<qint64,QPair<bool,QByteArray>>  IcDataTransPkgWriteBuff_PkgItem;
#define T_WritePriv( o )  T_ObjCast( IcDataTransPkgWriteBuffPriv*,o )
class QXPACK_IC_HIDDEN  IcDataTransPkgWriteBuffPriv : public QObject {
    Q_OBJECT
private:
    IcDataTransPkgWriteBuff   *m_parent;
    QList< IcDataTransPkgWriteBuff_PkgItem > m_pkg_queue;
    QSharedPointer<QIODevice>  m_io_dev_sp;
    bool m_is_pending_post_call;
protected:
                void  postPendingPostData( );
    Q_INVOKABLE void  postData ( );
    Q_SLOT      void  ioDev_onBytesWritten( qint64 );
public :
    explicit IcDataTransPkgWriteBuffPriv( IcDataTransPkgWriteBuff*,  QSharedPointer<QIODevice> &dev, QObject *pa = Q_NULLPTR );
    virtual ~IcDataTransPkgWriteBuffPriv( ) Q_DECL_OVERRIDE;
    void  clear( );
    bool  post ( const QVector<QPair<bool,QByteArray>> &ba_list, bool is_pkg = true );
};

// ============================================================================
// ctor
// ============================================================================
IcDataTransPkgWriteBuffPriv :: IcDataTransPkgWriteBuffPriv (
  IcDataTransPkgWriteBuff *parent,  QSharedPointer<QIODevice> &dev, QObject *pa
) : QObject( pa ), m_is_pending_post_call( false )
{
    m_parent = parent;

    // ------------------------------------------------------------------------
    // attach to the QIODevice write signals
    // ------------------------------------------------------------------------
    if ( dev.data() == Q_NULLPTR ) { qFatal("The QIODevice pointer is null!"); }
    m_io_dev_sp = dev;
    QObject::connect (
        m_io_dev_sp.data(), SIGNAL(bytesWritten(qint64)),
        this, SLOT(ioDev_onBytesWritten(qint64)), Qt::QueuedConnection
    );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransPkgWriteBuffPriv :: ~IcDataTransPkgWriteBuffPriv( )
{
    // ------------------------------------------------------------------------
    // drop all connection about QIODevice
    // ------------------------------------------------------------------------
    QObject::disconnect( m_io_dev_sp.data(), Q_NULLPTR, this, Q_NULLPTR );
}

// ============================================================================
// do pending post call
// ============================================================================
void  IcDataTransPkgWriteBuffPriv :: postPendingPostData( )
{
    // ------------------------------------------------------------------------
    //  make a queued call, and setup a flag to prevent repeat post.
    // ------------------------------------------------------------------------
    if ( ! m_is_pending_post_call ) {
        m_is_pending_post_call = true;
        QMetaObject::invokeMethod ( this, "postData", Qt::QueuedConnection );
    }
}

// ============================================================================
// slot: handle the bytes written signals
// ============================================================================
void IcDataTransPkgWriteBuffPriv :: ioDev_onBytesWritten( qint64 )
{
    // ------------------------------------------------------------------------
    //  the IO device has been wrote data, if current no pending call, directly
    // call the postData() to determine next step
    // ------------------------------------------------------------------------
    if ( ! m_is_pending_post_call ) {
        this->postData();
    }
}

// ============================================================================
// clear all package in the queue
// ============================================================================
void  IcDataTransPkgWriteBuffPriv :: clear( )
{
    m_pkg_queue = QList<IcDataTransPkgWriteBuff_PkgItem>();
}

// ============================================================================
// [ invokable ] post the data
// ============================================================================
void  IcDataTransPkgWriteBuffPriv :: postData()
{
    // ------------------------------------------------------------------------
    // unmark the pending call flag, here we are solving the post call
    // ------------------------------------------------------------------------
    if ( m_is_pending_post_call ) { m_is_pending_post_call = false; }
    if ( m_pkg_queue.isEmpty() ) { return; } // all package solved.

    // ------------------------------------------------------------------------
    // handle the first package
    // ------------------------------------------------------------------------
    IcDataTransPkgWriteBuff_PkgItem &item = m_pkg_queue.first();
    qint64 d_cnt = item.second.second.size();
    qint64 s_cnt = item.first;          // sent bytes
    bool is_req_cw = false;
    bool is_need_emit_sent = false;

    if ( s_cnt < d_cnt ) {    // has remain bytes to send
        qint64 rem_cnt = d_cnt - s_cnt;
        qint64 wr_cnt  = m_io_dev_sp->write( item.second.second.constData() + s_cnt, rem_cnt );

        if ( wr_cnt == 0 ) {  // send buffer full ? do nothing, wait for next written signals
            ;
        } else if ( wr_cnt >= rem_cnt ) {  // this package sent OK
            is_need_emit_sent = item.second.first;  // first is a boolean means whether it is a pckage.
            m_pkg_queue.pop_front();       // we try to continously write
            is_req_cw = true;

        } else if ( wr_cnt <  rem_cnt ) {  // remain some bytes
            item.first += wr_cnt;          // wait for a written signal, and continously write

        } else {  // -1 means an error
            qFatal("Write IODevice failed.");
        }
    } else {  // this package was sent, drop it, try next package.
        is_need_emit_sent = item.second.first;
        m_pkg_queue.pop_front();
        is_req_cw = true;
    }

    // ------------------------------------------------------------------------
    // check if the need post a next call for do send bytes
    // ------------------------------------------------------------------------
    if ( is_req_cw ) {
        this->postPendingPostData();
    }
    if ( is_need_emit_sent ) {
        QMetaObject::invokeMethod( m_parent, "packageSent", Qt::QueuedConnection );
    }
}

// ============================================================================
// post the data into queue
// ============================================================================
bool  IcDataTransPkgWriteBuffPriv :: post (
    const QVector<QPair<bool,QByteArray>> &ba_list, bool is_pkg
) {
    if ( ba_list.isEmpty() ) { return false; }

    // ------------------------------------------------------------------------
    // check if there is empty data in vector
    // ------------------------------------------------------------------------
    bool has_empty_data = false;
    QVector<QPair<bool,QByteArray>>::const_iterator c_itr = ba_list.constBegin();
    while ( c_itr != ba_list.constEnd()) {
        const QPair<bool,QByteArray> &item = (*c_itr ++ );
        if ( item.second.isEmpty() ) { has_empty_data = true; break; }
    }
    if ( has_empty_data ) { return false; }

    // ------------------------------------------------------------------------
    // now push all data into queue
    // ------------------------------------------------------------------------
    c_itr = ba_list.constBegin();
    while ( c_itr != ba_list.constEnd()) {
        const QPair<bool,QByteArray> &item = (*c_itr ++ );
        if ( is_pkg ) { // the item.first ( true means is data, false means non-data )
            m_pkg_queue.push_back( IcDataTransPkgWriteBuff_PkgItem( 0, item ));
        } else {       // override the flag
            m_pkg_queue.push_back( IcDataTransPkgWriteBuff_PkgItem( 0, QPair<bool,QByteArray>( false, item.second )));
        }
    }

    this->postPendingPostData();
    return true;
}

// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcDataTransPkgWriteBuff :: IcDataTransPkgWriteBuff ( QSharedPointer<QIODevice> &dev, QObject *pa )
  : QObject( pa )
{
    m_obj = qxpack_ic_new( IcDataTransPkgWriteBuffPriv, this, dev );
}

// ============================================================================
// dtor
// ============================================================================
IcDataTransPkgWriteBuff :: ~IcDataTransPkgWriteBuff ( )
{
    qxpack_ic_delete( m_obj, IcDataTransPkgWriteBuffPriv );
}

// ============================================================================
// clear all pending package
// ============================================================================
void  IcDataTransPkgWriteBuff :: clear( ) { T_WritePriv( m_obj )->clear(); }

// ============================================================================
// post the bytearray
// ============================================================================
bool  IcDataTransPkgWriteBuff :: post ( const QByteArray &ba, bool is_pkg )
{
    if ( ba.isEmpty() ) { return false; }
    bool is_post = false;

    QVector<QPair<bool,QByteArray>> ba_list;
    if ( this->packData( ba, ba_list )) {
        is_post = T_WritePriv( m_obj )->post( ba_list, is_pkg );
    }
    return is_post;
}

// ============================================================================
// [ virtual ] append a header into hdr bytearray
// ============================================================================
bool   IcDataTransPkgWriteBuff :: packData(
    const QByteArray &dat, QVector<QPair<bool,QByteArray> > &ba_list
) {
    QByteArray def_hdr( sizeof( IcDataTransPkg_HeaderWrap ), 0 );
    new( def_hdr.data() ) IcDataTransPkg_HeaderWrap( dat.constData(), quint64( dat.size()) );

    ba_list.reserve( 2 );
    ba_list.append( QPair<bool,QByteArray>( false, def_hdr ));
    ba_list.append( QPair<bool,QByteArray>( true, dat ));

    return true;
}





}
#include "qxpack_ic_datatrans_buff.moc"
#endif
