#ifndef USBDEV_DEVCTL_CXX
#define USBDEV_DEVCTL_CXX

#include "usbdev/main/usbdev_profile.hxx"
#include "usbdev/main/usbdev_framedata.hxx"
#include "usbdev/main/usbdev_statusdata.hxx"
#include "usbdev/main/usbdev_config.hxx"
#include "usbdev/main/usbdev_cache.hxx"
#include "usbdev_devctl.hxx"
#include <QThread>
#include <QTimer>
#include <QMetaObject>
#include <QDebug>
#include <QElapsedTimer>
#include <QAtomicInt>
#include <memory>
#include <QByteArray>
#include "nwkusbobj2.hxx"
#include "usbdev/common/usbdev_memcntr.hxx"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjcreator_priv.hxx"
#include "qxpack/indcom/common/qxpack_ic_queuetemp.hpp"
#include "qxpack/indcom/sys/qxpack_ic_rmtobjsigblocker_priv.hxx"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <memory>
#pragma execution_character_set("utf-8")

namespace UsbDev {

static std::shared_ptr<spdlog::logger> logger=NULL;

// ////////////////////////////////////////////////////////////////////////////
// helper function
// ////////////////////////////////////////////////////////////////////////////
static QString buffToQStr(const char* buff,size_t length)
{
    QString str;
    QByteArray byteArray= QByteArray::fromRawData(buff,length);
    for(int i = 0; i< byteArray.length(); i++){
       QString str1 = QString("%1").arg(i,2,10, QLatin1Char('0'));
       uchar temp=static_cast<uchar>(byteArray[i]);
       QString str2 = QString("%1").arg(temp,2,16, QLatin1Char('0'));
       str.append(QString("%1:%2 ").arg(str1).arg(str2));
    }
    return qPrintable(str);
}

static void  gPutInt32_Le( unsigned char *buff, qint32 v )
{
    buff[0] = quint8( v & 0x0ff );          buff[1] = quint8(( v >> 8 ) & 0x0ff );
    buff[2] = quint8(( v >> 16 ) & 0x0ff ); buff[3] = quint8(( v >> 24) & 0x0ff );
}

static void  gPutInt16_Le( unsigned char *buff, qint16 v )
{
    buff[0] = quint8( v & 0x0ff );          buff[1] = quint8(( v >> 8 ) & 0x0ff );
}

static void  gPutInt32( unsigned char *buff, qint32 v)
{
        gPutInt32_Le( buff, v );
}

static void  gPutInt16( unsigned char *buff, qint16 v)
{
        gPutInt16_Le( buff, v );
}



// ////////////////////////////////////////////////////////////////////////////
//
//                    worker ( running in a QThread )
//
// ////////////////////////////////////////////////////////////////////////////
class USBDEV_HIDDEN DevCtl_Worker : public QObject {
    Q_OBJECT
private:
    Profile m_profile;
    Config m_config;
    StaticCache m_staticCache[3];
    MoveCache m_moveCache[3];
    SciPack::NwkUsbObj2 *m_usb_dev;
    QElapsedTimer m_elapse_tmr;
/*    quint32 m_trg_cntr; */quint32 m_vid_pid, m_cfg_id; bool m_is_video_on;
    QAtomicInt  m_video_trg_called;
    QAtomicInt  m_status_trg_called;
    DevCtl::WorkStatus  m_wks;
    quint32 m_key_bmp;
protected:
    auto  cmdComm_bulkOutSync( const unsigned char *buff, int buff_sz ) -> bool;
    auto  cmdComm_strInSync  (       unsigned char *buff, int buff_sz ) -> bool;
    auto  cmdComm_bulkInSync (       unsigned char *buff, int buff_sz ) -> bool;
    auto  isDeviceWork( ) -> bool
    {
        if ( m_usb_dev == nullptr ) { return false; }
        if ( m_usb_dev->status() != SciPack::NwkUsbObj2::StatusID_S_OK ) { return false; }
        return true;
    }

public :
    explicit DevCtl_Worker( quint32 vid_pid, quint32 cfg, QObject* pa = Q_NULLPTR );
    virtual ~DevCtl_Worker( ) Q_DECL_OVERRIDE;

    Q_INVOKABLE void  init( bool req_emit = true );
    Q_SLOT      void  video_trigger_di( );
    Q_SLOT      void  status_trigger_di( );
    Q_INVOKABLE void  forceReadProfile( void *p ) { Profile *pf = reinterpret_cast<Profile*>(p); *pf = m_profile; }
    Q_INVOKABLE int   forceReadWorkStatus( ) { return m_wks; }  // since 0.2.1  try force check workstatus
    Q_INVOKABLE bool  cmd_ReadProfile( bool req_emit = true );
    Q_INVOKABLE bool  cmd_ReadConfig( bool req_emit = true );
    Q_INVOKABLE StaticCache*  cmd_ReadStaticCache();
    Q_INVOKABLE MoveCache*  cmd_ReadMoveCache();
    Q_INVOKABLE bool  cmd_ReadStatusData( );
    Q_INVOKABLE bool  cmd_ReadFrameData ( );
    Q_INVOKABLE bool  cmd_TurnOnVideo ( );
    Q_INVOKABLE bool  cmd_TurnOffVideo( );
    Q_INVOKABLE bool  cmd_ClearCache( );
    Q_INVOKABLE bool  cmd_GeneralCmd( QByteArray ba  ,QString funcName,quint32 dataLen);

    Q_SIGNAL void  workStatusChanged    ( int );
    Q_SIGNAL void  newConfig            ( const UsbDev::Config &);
    Q_SIGNAL void  newProfile           ( const UsbDev::Profile &    );
    Q_SIGNAL void  newFrameData         ( const UsbDev::FrameData &  );
    Q_SIGNAL void  newStatusData        ( const UsbDev::StatusData & );
    Q_SIGNAL void  newStaticCache       ( const UsbDev::StaticCache* );
    Q_SIGNAL void  videoStatusChanged   ( bool );
    Q_SIGNAL void  updateInfo           ( QString );
    Q_SIGNAL void  updateIOInfo         ( QString );
    Q_SIGNAL void  updateRefreshInfo    ( QString );
    Q_SIGNAL void  updateRefreshIOInfo    ( QString );

    Q_INVOKABLE bool  cmd_readUsbEEPROM ( char *buff, int size, int eeprom_addr  ); // since 0.5
    Q_INVOKABLE bool  cmd_writeUsbEEPROM( const char *buff, int size, int eeprom_addr );
};

// ============================================================================
// ctor
// ============================================================================
DevCtl_Worker :: DevCtl_Worker ( quint32 vid_pid, quint32 cfg_id, QObject *pa ) : QObject( pa )
{
    m_usb_dev = nullptr; /*m_trg_cntr = 0;*/
    m_vid_pid = vid_pid; m_cfg_id = cfg_id; m_is_video_on = false;
    m_video_trg_called.store(0);
    m_status_trg_called.store(0);
    m_key_bmp = 0;
    m_wks = DevCtl::WorkStatus_S_Disconnected;
}

// ============================================================================
// dtor
// ============================================================================
DevCtl_Worker :: ~DevCtl_Worker ( )
{
    if ( m_usb_dev != nullptr ) {
        this->cmd_TurnOffVideo();
        usbdev_delete( m_usb_dev, SciPack::NwkUsbObj2 );
        emit this->workStatusChanged( DevCtl::WorkStatus_S_Disconnected );
    }
}

// ============================================================================
// method: handle the initialize 初始化的时候读取一次. 读取profile 和 config
// ============================================================================
void   DevCtl_Worker :: init( bool req_emit )
{
    if ( m_usb_dev == nullptr ) {
        emit updateInfo(QString("intialize connetion.VIdPId:%1.").arg(QString::number(m_vid_pid,16)));
        m_wks = DevCtl::WorkStatus_S_ConnectToDev;
        if ( req_emit ) { emit this->workStatusChanged( DevCtl::WorkStatus_S_ConnectToDev ); }
        m_usb_dev = usbdev_new( SciPack::NwkUsbObj2, m_vid_pid, uint8_t( m_cfg_id ), 0xff000000, 0, 0 );//连接实例

        if ( m_usb_dev->status() == SciPack::NwkUsbObj2::StatusID_S_OK ) {
            m_wks = DevCtl::WorkStatus_S_OK;
            if ( req_emit ) { emit this->workStatusChanged( DevCtl::WorkStatus_S_OK ); }
            emit updateInfo("connection succeed.");
            this->cmd_ClearCache();
            this->cmd_ReadProfile( req_emit );
//            this->cmd_ReadConfig (req_emit);                          //以后要加上
        } else {
            m_wks = DevCtl::WorkStatus_E_UnExpected;
            if ( req_emit ) { emit this->workStatusChanged( DevCtl::WorkStatus_E_UnExpected ); }
            emit updateInfo("connection UnExpected.");
            usbdev_delete( m_usb_dev, SciPack::NwkUsbObj2 );
            m_usb_dev = nullptr;
            m_wks = DevCtl::WorkStatus_S_Disconnected;
            if ( req_emit ) { emit this->workStatusChanged( DevCtl::WorkStatus_S_Disconnected ); }
            emit updateInfo("disconnected.");
        }
    }
}

// ============================================================================
// method: work trigger   轮询式读取.读取视频和轮询数据.
// ============================================================================
void   DevCtl_Worker :: video_trigger_di( )
{
    if ( m_video_trg_called.loadAcquire() > 5 ) { return; }
    m_video_trg_called.fetchAndAddOrdered(1);

    QMetaObject::invokeMethod( this, "cmd_ReadFrameData", Qt::QueuedConnection );

}

void   DevCtl_Worker :: status_trigger_di( )
{
    if ( m_status_trg_called.loadAcquire() > 5 ) { return; }
    m_status_trg_called.fetchAndAddOrdered(1);

   QMetaObject::invokeMethod( this, "cmd_ReadStatusData", Qt::QueuedConnection );
}

// ============================================================================
// output the buffer to usb client
// ============================================================================
bool    DevCtl_Worker :: cmdComm_bulkOutSync( const unsigned char *buff, int buff_sz)
{
    SciPack::NwkUsbObj2::DataPacket  pkg;
    pkg.dat_id = 0; pkg.dat_ptr = const_cast<unsigned char*>( buff ); pkg.dat_size = buff_sz;
    bool ret=m_usb_dev->bulkTransSync( SciPack::NwkUsbObj2::PipeTypeID_BlkBulkOut, & pkg ) == SCIPACK_S_OK;
//    QString msg="cmdComm_bulkOutSync:"+buffToQStr(reinterpret_cast<const char*>(buff),buff_sz);
//    logger->info(msg.toStdString());
    return ret;
}

// ============================================================================
// input the buffer from usb client
// ============================================================================
bool    DevCtl_Worker :: cmdComm_bulkInSync( unsigned char *buff, int buff_sz)
{
    SciPack::NwkUsbObj2::DataPacket pkg;
    pkg.dat_id = 0; pkg.dat_ptr = buff; pkg.dat_size = buff_sz;
    bool ret=( m_usb_dev->bulkTransSync( SciPack::NwkUsbObj2::PipeTypeID_BlkBulkIn, & pkg ) == SCIPACK_S_OK );
//    QString msg="cmdComm_bulkInSync:"+buffToQStr(reinterpret_cast<const char*>(buff),buff_sz);
//    logger->info(msg.toStdString());
    return ret;
}

// ============================================================================
// input the buffer from usb client
// ============================================================================
bool    DevCtl_Worker :: cmdComm_strInSync( unsigned char *buff, int buff_sz )
{
    SciPack::NwkUsbObj2::DataPacket pkg;
    pkg.dat_id = 0; pkg.dat_ptr = buff; pkg.dat_size = buff_sz;
    bool ret=( m_usb_dev->bulkTransSync( SciPack::NwkUsbObj2::PipeTypeID_StrBulkIn, & pkg ) == SCIPACK_S_OK );
//    QString msg=buffToQStr(reinterpret_cast<const char*>(buff),20);
//    logger->info(msg.toStdString());
    return ret;
}


// ============================================================================
// cmd: read the profile from device ok
// ============================================================================
bool   DevCtl_Worker :: cmd_ReadProfile( bool req_emit )
{
    updateInfo("read Profile.");
//    if ( ! this->isDeviceWork()) { return false; }
    unsigned char buff[512]={0}; bool ret = true;

    buff[0] = 0x5a; buff[1] = 0xf0;
    updateIOInfo(QString("W:")+buffToQStr(reinterpret_cast<const char*>(buff),2));
    ret = this->cmdComm_bulkOutSync( buff, sizeof( buff ) );
    if ( ! ret ) { updateInfo("send read profile command failed."); }

    if ( ret ) {
        ret = this->cmdComm_bulkInSync( buff, sizeof( buff ));
        if ( ! ret ) { updateInfo("recv. profile data failed."); }
    }
    if ( ret ) {
        updateInfo("recv. profile data succeeded.");
        updateIOInfo(QString("R:")+buffToQStr(reinterpret_cast<const char*>(buff),72));
        Profile profile( QByteArray::fromRawData( reinterpret_cast<const char*>(buff), sizeof(buff)));
        if ( ! profile.isEmpty()) { if ( req_emit ){ emit this->newProfile( profile );} }
        m_profile = profile;
    }

    return ret;
}


// ============================================================================
bool   DevCtl_Worker :: cmd_ReadConfig( bool req_emit )
{
    updateInfo("read Config.");
    if ( !this->isDeviceWork())  updateInfo("no connection.");
    bool ret = true;
    int frameSize=512;
    int totalFrame=ceil(double(Config::dataLen())/frameSize);
    int fragment=Config::dataLen()-frameSize*(totalFrame-1);
    unsigned char* dataPtr=new unsigned char[totalFrame*frameSize];
    for(int i=0;i<totalFrame;i++)
    {

        unsigned char buff[512];
        buff[0] = 0x5a; buff[1] = 0xf1;buff[2]=totalFrame;buff[3]=i;
        updateIOInfo(QString("W:")+buffToQStr(reinterpret_cast<const char*>(buff),4));
        if ( this->isDeviceWork()) ret = this->cmdComm_bulkOutSync( buff, sizeof( buff ) );
        if ( ! ret ) { updateInfo(QString("send read config  %0nd segment command failed.").arg(QString::number(i))); }

        if(ret&&this->isDeviceWork())
        {
            if(i!=totalFrame-1)
            {
                ret = this->cmdComm_bulkInSync( dataPtr+512*i, 512 );
                if ( ! ret ) { updateInfo(QString("recv. config  %0nd segment data failed.").arg(QString::number(i)));}
                else   updateIOInfo(QString("Config %0nd segment R:").arg(QString::number(i))+buffToQStr(reinterpret_cast<const char*>(dataPtr+512*i),512));
            }
            else
            {
                ret = this->cmdComm_bulkInSync( dataPtr+512*i, 512);
                if ( ! ret ) { updateInfo(QString("recv. config  %0nd segment data failed.").arg(QString::number(i)));}
                else   updateIOInfo(QString("Config %0nd segment R:").arg(QString::number(i))+buffToQStr(reinterpret_cast<const char*>(dataPtr+512*i),fragment));
            }
        }
    }

    if ( ret&&this->isDeviceWork()) {
        updateInfo("recv. config data succeeded.");
        updateIOInfo(QString("Config R:")+buffToQStr(reinterpret_cast<const char*>(dataPtr),Config::dataLen()));
        Config config( QByteArray::fromRawData( reinterpret_cast<const char*>(dataPtr), sizeof(Config::dataLen())));
        if ( ! config.isEmpty()) { if ( req_emit ){ emit this->newConfig(config); }}
        m_config = config;
    }
    delete [] dataPtr;
    return ret;
}

StaticCache* DevCtl_Worker::cmd_ReadStaticCache()
{
    updateInfo("read static cache.");
    if(!this->isDeviceWork()){return Q_NULLPTR;}
    unsigned char buff[512]={0};
    bool ret = true;
    if(ret)
    {
        buff[0]=0x5a;buff[1]=0xf4;
        updateIOInfo(QString("W:")+buffToQStr(reinterpret_cast<const char*>(buff),2));
        ret=this->cmdComm_bulkOutSync(buff,sizeof(buff));
        if(!ret){updateInfo("send readStaticCache command failed.");}
    }
    if(ret)
    {
        ret=this->cmdComm_bulkInSync(buff,sizeof(buff));
        if(!ret){updateInfo("recv staticCache data failed.");}

    }
    if(ret)
    {
        updateIOInfo(QString("R:")+buffToQStr(reinterpret_cast<const char*>(buff),sizeof(m_staticCache))+"");
        memcpy(m_staticCache,buff,sizeof(m_staticCache));
        return m_staticCache;
    }
    return Q_NULLPTR;

}

MoveCache* DevCtl_Worker::cmd_ReadMoveCache()
{
    updateInfo("read move cache.");
    if(!this->isDeviceWork()){return Q_NULLPTR;}
    unsigned char buff[512]={0};
    bool ret = true;
    if(ret)
    {
        buff[0]=0x5a;buff[1]=0xf5;
        updateIOInfo(QString("W:")+buffToQStr(reinterpret_cast<const char*>(buff),2));
        ret=this->cmdComm_bulkOutSync(buff,sizeof(buff));
        if(!ret){updateInfo("send readMoveCache command failed.");}
    }
    if(ret)
    {
        ret=this->cmdComm_bulkInSync(buff,sizeof(buff));
        if(!ret){updateInfo("recv moveCache data failed.");}
    }
    if(ret)
    {
        updateIOInfo(QString("R:")+buffToQStr(reinterpret_cast<const char*>(buff),sizeof(m_moveCache))+"");
        memcpy(m_moveCache,buff,sizeof(m_moveCache));
        return m_moveCache;
    }
    return Q_NULLPTR;
}


// ============================================================================
// cmd: read the status data from device ok
// ============================================================================
bool   DevCtl_Worker :: cmd_ReadStatusData()
{
    if ( m_status_trg_called.loadAcquire() > 0 ) { m_status_trg_called.fetchAndSubOrdered(1); }
    if ( ! this->isDeviceWork()) {
        updateRefreshInfo("device not working not gonna read StatusData.");
        return false;
    }
    updateRefreshInfo("ReadStatus.");
    unsigned char buff[512]={0}; bool ret = true;
    if ( ret ) {
        buff[0] = 0x5A; buff[1] = 0xF3;
        updateRefreshIOInfo(QString("W:")+buffToQStr(reinterpret_cast<const char*>(buff),2));
        ret = this->cmdComm_bulkOutSync( buff, sizeof( buff ));
        if ( ! ret  ) {
            updateRefreshInfo("send read status command failed.");
        }
    }
    if ( ret ) {
        ret = this->cmdComm_bulkInSync( buff, sizeof( buff ));
        if ( ! ret ) { updateRefreshInfo("recv. status data failed."); }
    }
    if ( ret ) {
        updateRefreshIOInfo(QString("R:")+buffToQStr(reinterpret_cast<const char*>(buff),64)+"");
        StatusData sd( QByteArray::fromRawData( reinterpret_cast<const char*>(buff),sizeof(buff)));
        if ( ! sd.isEmpty()) {
            emit this->newStatusData( sd );
        }
    }
    return ret;
}

// ============================================================================
// cmd: read the front video data
// ============================================================================
bool  DevCtl_Worker :: cmd_ReadFrameData()
{
    static int count=0;
    if ( m_video_trg_called.loadAcquire() > 0 ) { m_video_trg_called.fetchAndSubOrdered(1); }
    if ( ! this->isDeviceWork() || ! m_is_video_on || m_profile.isEmpty()) { updateRefreshInfo("no camera."); return false; }
    updateRefreshInfo("read frame:"+QString::number(count)+"");
    bool ret = true;
    QSize sz = m_profile.videoSize();
    QByteArray ba( sz.height()*sz.width(), 0 );
    ret = this->cmdComm_strInSync( reinterpret_cast<unsigned char*>( ba.data()), ba.size());
    if ( ret ) {
        updateRefreshIOInfo(QString("R:")+buffToQStr(reinterpret_cast<const char*>(ba.data()),20)+"");
//        if(count%10==0)
//        {
//            QFile file(QString(R"(./videoData/)")+QString::number(count));
//            file.open(QIODevice::ReadWrite);
//            file.write(ba);
//            file.close();
//            QImage img((uchar*)ba.data(),640,480,QImage::Format::Format_Grayscale8);
//            img.save(QString(R"(./videoData/)")+QString::number(count)+".bmp");
//        }
        FrameData fd( ba );
        emit this->newFrameData( fd );
        count++;

    }
    else
    {
        updateRefreshInfo("recv frame data failed.");
    }
    return ret;

}

//bool  DevCtl_Worker :: cmd_ClearCache()
//{
//    if ( ! this->isDeviceWork()) { updateInfo("no connection!");return false; }
////    if ( m_is_video_on ) { return true; }
//    bool ret = true;
//    updateInfo("打开摄像头.");
//    if ( ret ) { // turn on EP2
//        SciPack::NwkUsbObj2::SetupPacket pkg;
//        pkg.m_req_type = 0x40; pkg.m_req = 0xb2; pkg.m_value = 0;
//        pkg.m_index = 0x01; pkg.m_length = 0;
//        if ( m_usb_dev->ctlTransSync( & pkg, nullptr, nullptr ) != SCIPACK_S_OK ) {
//            updateInfo("control transfer failed, can not open EP2 IN.");
//            ret = false;
//        }
//    }
//    if ( ret ) { // turn on video
//        unsigned char buff[512];
//        buff[0] = 0x5a; buff[1] = 0x70; buff[2] = 0x00; buff[3] = 0x01;
//        updateIOInfo(buffToQStr(reinterpret_cast<const char*>(buff),4));
//        ret = this->cmdComm_bulkOutSync( buff, sizeof( buff ));
//        if ( ! ret ) { updateInfo("send video-on cmd failed."); }
//    }
//    if ( ret ) {
//        m_is_video_on = true; m_elapse_tmr.start();
//        updateInfo("摄像头打开成功.");
//        emit this->videoStatusChanged( true );
//    }
//    return ret;
//}


// ============================================================================
// cmd: the video open or close control  ok
// ============================================================================
bool  DevCtl_Worker :: cmd_TurnOnVideo()
{
    cmd_ClearCache();
    if ( ! this->isDeviceWork()) { updateInfo("no connection!");return false; }
//    if ( m_is_video_on ) { return true; }
    bool ret = true;
    updateInfo("open camera.");
    if ( ret ) { // turn on EP2
        SciPack::NwkUsbObj2::SetupPacket pkg;
        pkg.m_req_type = 0x40; pkg.m_req = 0xb2; pkg.m_value = 0;
        pkg.m_index = 0x01; pkg.m_length = 0;
        if ( m_usb_dev->ctlTransSync( & pkg, nullptr, nullptr ) != SCIPACK_S_OK ) {
            updateInfo("control transfer failed, can not open EP2 IN.");
            ret = false;
        }
    }
    if ( ret ) { // turn on video
        unsigned char buff[512];
        buff[0] = 0x5a; buff[1] = 0x70; buff[2] = 0x00; buff[3] = 0x01;
        updateIOInfo(buffToQStr(reinterpret_cast<const char*>(buff),4));
        ret = this->cmdComm_bulkOutSync( buff, sizeof( buff ));
        if ( ! ret ) { updateInfo("send video-on cmd failed."); }
    }
    if ( ret ) {
        m_is_video_on = true; m_elapse_tmr.start();
        updateInfo("open camera succeed.");
        emit this->videoStatusChanged( true );
    }
    return ret;
}

// ============================================================================
// turn off the video  ok
// ============================================================================
bool  DevCtl_Worker :: cmd_TurnOffVideo()
{
    if ( ! this->isDeviceWork()) { updateInfo("no connection!");return false; }
//    if ( ! m_is_video_on ) { return true; }
    bool ret = true;
    updateInfo("turn off camera.");
    if ( ret ) { // turn off video
        unsigned char buff[512];
        buff[0] = 0x5a; buff[1] = 0x70; buff[2] = 0x00; buff[3] = 0x00;
        QString msg=buffToQStr(reinterpret_cast<const char*>(buff),4);
        updateIOInfo(msg);
        ret = this->cmdComm_bulkOutSync( buff, sizeof( buff ));
        if ( ! ret ) { updateInfo("send video-off cmd failed."); }
    }
    if ( ret ) { // turn off EP2
        SciPack::NwkUsbObj2::SetupPacket pkg;
        pkg.m_req_type = 0x40; pkg.m_req = 0xb2; pkg.m_value = 0;
        pkg.m_index = 0x02; pkg.m_length = 0;
        if ( m_usb_dev->ctlTransSync( & pkg, nullptr, nullptr ) != SCIPACK_S_OK ) {
            updateInfo("control transfer failed, can not close EP2 IN.");
            ret = false;
        }
    }
    if ( ret ) {
        m_is_video_on = false; m_elapse_tmr.invalidate();
        emit this->videoStatusChanged( false );
    }

    return ret;
}

bool DevCtl_Worker::cmd_ClearCache()
{
    if ( ! this->isDeviceWork()) { updateInfo("no connection!");return false; }
    bool ret = true;
    updateInfo("clear Cache.");
    if ( ret ) { //clear Cache
        SciPack::NwkUsbObj2::SetupPacket pkg;
        pkg.m_req_type = 0x40; pkg.m_req = 0xb2; pkg.m_value = 0;
        pkg.m_index = 0x03; pkg.m_length = 0;
        if ( m_usb_dev->ctlTransSync( & pkg, nullptr, nullptr ) != SCIPACK_S_OK ) {
            updateInfo("Can't clear Cache.");
            ret = false;
        }
    }
    if ( ret ) {
        updateInfo("clear cache done.");
    }
    return ret;
}



// ============================================================================
// cmd: save the motor configuration data
// ============================================================================

bool DevCtl_Worker::cmd_GeneralCmd(QByteArray ba, QString funcName,quint32 dataLen)
{
    QString msg=buffToQStr(reinterpret_cast<const char*>(ba.data()),dataLen);
    emit updateInfo(funcName);
    emit updateIOInfo(buffToQStr(reinterpret_cast<const char*>(ba.data()),dataLen));
    if ( ! this->isDeviceWork()) { updateInfo("no connection!");return false; }
    bool ret = this->cmdComm_bulkOutSync((unsigned char*) ba.data(), ba.size() );
    if(ret)
    {
        emit updateInfo("send"+ funcName+" succeed.");
    }
    else
    {
        emit updateInfo("send "+ funcName+" failed.");
    }
    return true;
}



// ============================================================================
// eeprom read/write
// ============================================================================
auto     DevCtl_Worker :: cmd_readUsbEEPROM ( char *buff, int size, int eeprom_addr  ) -> bool
{
    if ( ! this->isDeviceWork()) { return false; }

    SciPack::NwkUsbObj2::SetupPacket setup_pkg;
    setup_pkg.m_req_type = 0xc0;   setup_pkg.m_req   = 0xb0;
    setup_pkg.m_index    = 0x5103; setup_pkg.m_value = uint16_t( eeprom_addr );
    setup_pkg.m_length   = uint16_t( size );

    SciPack::NwkUsbObj2::DataPacket  data_pkg;
    data_pkg.dat_ptr = buff; data_pkg.dat_size = size;
    intptr_t bytes_trans;
    auto ret = ( m_usb_dev->ctlTransSync( & setup_pkg, & data_pkg, & bytes_trans ) == SCIPACK_S_OK );
//    if ( ! ret ) { spdlog::warn("read USB EEPROM cmd failed!"); }
    if ( ret ) {
        updateInfo( QString("readEEPROM: %1 address OK.").arg(QString::number(eeprom_addr)));
    }
    return ret;
}

auto     DevCtl_Worker :: cmd_writeUsbEEPROM( const char *buff, int size, int eeprom_addr ) -> bool
{
    if ( ! this->isDeviceWork()) { return false; }

    SciPack::NwkUsbObj2::SetupPacket setup_pkg;
    setup_pkg.m_req_type = 0x40;   setup_pkg.m_req = 0xb0;
    setup_pkg.m_index    = 0x5183; setup_pkg.m_value = uint16_t( eeprom_addr );
    setup_pkg.m_length   = uint16_t( size );
    SciPack::NwkUsbObj2::DataPacket data_pkg;
    data_pkg.dat_ptr = const_cast<void*>( reinterpret_cast<const void*>( buff )); data_pkg.dat_size = size;
    intptr_t bytes_trans;
    auto ret = ( m_usb_dev->ctlTransSync( & setup_pkg, & data_pkg, & bytes_trans ) == SCIPACK_S_OK );
//    if ( ! ret ) { spdlog::warn("write USB EEPROM cmd failed!"); }
    if ( ret ) {
        updateInfo(  QString("writeEEPROM: %1 address OK.").arg(QString::number(eeprom_addr )));
    }
    return ret;
}



// ////////////////////////////////////////////////////////////////////////////
//
//                       private object
//
// ////////////////////////////////////////////////////////////////////////////
typedef QxPack::IcArrayQueueTemp<UsbDev::StatusData,3>   DevCtl_StatusDataQueue;
typedef QxPack::IcArrayQueueTemp<UsbDev::FrameData,3>    DevCtl_FrameDataQueue;
#define T_PrivPtr( o )  usbdev_objcast( DevCtlPriv*, o )
class USBDEV_HIDDEN DevCtlPriv : public QObject {
    Q_OBJECT
private:
    QThread  *m_t_worker; DevCtl_Worker *m_wkr;  // worker thread and worker
    QThread  *m_t_tmr;    QTimer *m_video_tmr;  QTimer *m_status_tmr;   // timer thread and timer
    Profile   m_profile;
    Config    m_config;
    DevCtl_StatusDataQueue  m_status_data_queue;
    DevCtl_FrameDataQueue   m_frame_data_queue;
    quintptr  m_status_data_emit_cntr, m_frame_data_emit_cntr;
    quint32   m_vid_pid, m_cfg_id;
    DevCtl::WorkStatus m_wkr_status;
    bool   m_is_inited;
protected:
    inline void  emitNewFrameData( )
    {
       if ( m_frame_data_emit_cntr < 1 ) {
           ++ m_frame_data_emit_cntr; emit this->newFrameData();
       }
    }

    inline void  emitNewStatusData( )

    {
        if ( m_status_data_emit_cntr < 1 ) {
            ++ m_status_data_emit_cntr; emit this->newStatusData();
        }
    }

           void  ensureWorker( bool );
           void  ensureTimer ( bool );

    //这几个SLOT被Devwoker连接,之后又会调用自己的SIGNAL,然后传递到最上层
    Q_SLOT void  wkr_onWorkStatusChanged( int );
    Q_SLOT void  wkr_onNewFrameData ( const UsbDev::FrameData & );
    Q_SLOT void  wkr_onNewStatusData( const UsbDev::StatusData & );
    Q_SLOT void  wkr_onNewProfile   ( const UsbDev::Profile & );
    Q_SLOT void  wkr_onNewConfig   ( const UsbDev::Config & );
    Q_SLOT void  wkr_onVideoStatusChanged( bool );


public:
    explicit DevCtlPriv( quint32 vid_pid, quint32 cfg );
    virtual ~DevCtlPriv( ) Q_DECL_OVERRIDE;
           auto  init( bool req_sync = false ) -> void;
    inline auto  isInited( ) const -> bool  { return m_is_inited; }
    inline auto  workStatus()   -> DevCtl::WorkStatus& { return m_wkr_status; }
    inline auto  vidPid() const -> quint32  { return m_vid_pid; }
    inline auto  cfgId () const -> quint32  { return m_cfg_id;  }
    inline auto  profile()      -> Profile& { return m_profile; }
    inline auto  config()       -> Config& { return m_config; }
    inline auto  wkrPtr()       -> DevCtl_Worker* { return m_wkr; }
    inline auto  takeNextPendingFrameData( UsbDev::FrameData &fd ) -> bool
    {
        if ( m_frame_data_emit_cntr > 0 ) { -- m_frame_data_emit_cntr; }
        bool ret = m_frame_data_queue.dequeue( fd );
        if ( ! m_frame_data_queue.isEmpty() ) {
//           QMetaObject::invokeMethod( this, "emitNewFrameData", Qt::QueuedConnection );
            this->emitNewFrameData();
        }
        return ret;
    }

    inline auto  takeNextPendingStatusData( UsbDev::StatusData &sd ) -> bool
    {
        if ( m_status_data_emit_cntr > 0 ) { -- m_status_data_emit_cntr; }
        bool ret = m_status_data_queue.dequeue( sd );
        if ( ! m_status_data_queue.isEmpty()) {
//            QMetaObject::invokeMethod( this, "emitNewStatusData", Qt::QueuedConnection );
            this->emitNewStatusData();
        }
        return ret;
    }

    Q_SIGNAL void  workStatusChanged( int );
    Q_SIGNAL void  newStatusData( );
    Q_SIGNAL void  newFrameData( );
    Q_SIGNAL void  newProfile( );
    Q_SIGNAL void  newConfig( );
    Q_SIGNAL void  updateInfo(QString );
    Q_SIGNAL void  updateIOInfo(QString);
    Q_SIGNAL void  updateRefreshInfo(QString );
    Q_SIGNAL void  updateRefreshIOInfo(QString);
};

// ============================================================================
// ctor
// ============================================================================
DevCtlPriv :: DevCtlPriv ( quint32 vid_pid, quint32 cfg )
{
    // init. all variables
    m_vid_pid = vid_pid; m_cfg_id = cfg;
    m_video_tmr = Q_NULLPTR;
    m_status_tmr = Q_NULLPTR;
    m_wkr = Q_NULLPTR;
    m_status_data_emit_cntr = m_frame_data_emit_cntr = 0;
    m_wkr_status = DevCtl::WorkStatus_S_Disconnected;
    m_is_inited  = false;

    // create the timer thread and work thread
    m_t_tmr = usbdev_new_qobj( QThread );
    QObject::connect( m_t_tmr, SIGNAL(finished()), m_t_tmr, SLOT(deleteLater()));
    m_t_tmr->start();
    m_t_worker = usbdev_new_qobj( QThread );
    QObject::connect( m_t_worker, SIGNAL(finished()), m_t_worker, SLOT(deleteLater()));
    m_t_worker->start();
}

// ============================================================================
// dtor
// ============================================================================
DevCtlPriv :: ~DevCtlPriv ()
{
    // drop old object that running in thread, quit() will trigger deleteLater()
    this->ensureTimer ( false );
    this->ensureWorker( false );
    m_t_tmr->quit();
    m_t_worker->quit();
}

// ============================================================================
// ensure the trigger timer
// ============================================================================
void  DevCtlPriv :: ensureTimer( bool sw )
{
    if ( sw ) {  // create timer in timer thread
        this->ensureTimer( false );
        m_video_tmr = qobject_cast<QTimer*>(
            QxPack::IcRmtObjCreator::createObjInThread (
                m_t_tmr, []( void *)->QObject*{
                    QTimer *tmr = usbdev_new_qobj( QTimer );
                    tmr->setInterval( 1000/30 );
                    tmr->setSingleShot( false );
                    return tmr;
                },this
            )
        );
        QMetaObject::invokeMethod( m_video_tmr, "start" );

        m_status_tmr = qobject_cast<QTimer*>(
            QxPack::IcRmtObjCreator::createObjInThread (
                m_t_tmr, []( void *)->QObject*{
                    QTimer *tmr = usbdev_new_qobj( QTimer );
                    tmr->setInterval( 1000/100 );
                    tmr->setSingleShot( false );
                    return tmr;
                },this
            )
        );
        QMetaObject::invokeMethod( m_status_tmr, "start" );

    } else {   // delete timer in timer thread
        if ( m_video_tmr != Q_NULLPTR ) {
//            QxPack::IcRmtObjSigBlocker::blockSignals ( m_t_tmr, true );  //影响退出.
            m_video_tmr->deleteLater();
            m_video_tmr = Q_NULLPTR;
        }
        if ( m_status_tmr != Q_NULLPTR ) {
//            QxPack::IcRmtObjSigBlocker::blockSignals ( m_t_tmr, true );  //影响退出.
            m_status_tmr->deleteLater();
            m_status_tmr = Q_NULLPTR;
        }
    }
}

// ============================================================================
// create the worker by spec. VID PID
// ============================================================================
void  DevCtlPriv :: ensureWorker( bool sw )
{
    if ( sw  ) {  // create worker in work thread
        this->ensureWorker( false );
        m_wkr = qobject_cast<DevCtl_Worker*>(
            QxPack::IcRmtObjCreator::createObjInThread (
                m_t_worker, [](void *ctxt )->QObject*{
                    DevCtlPriv *t_this = reinterpret_cast<DevCtlPriv*>(ctxt);
                    return usbdev_new_qobj( DevCtl_Worker, t_this->m_vid_pid, t_this->m_cfg_id );
                }, this
            )
        );
        QObject::connect( m_wkr, SIGNAL(workStatusChanged(int)), this, SLOT(wkr_onWorkStatusChanged(int)));
        QObject::connect( m_wkr, SIGNAL(newProfile( const UsbDev::Profile&)), this, SLOT(wkr_onNewProfile(const UsbDev::Profile&)));
        QObject::connect( m_wkr, SIGNAL(newConfig(const UsbDev::Config&)), this, SLOT(wkr_onNewConfig(const UsbDev::Config&)));
        QObject::connect( m_wkr, SIGNAL(newStatusData( const UsbDev::StatusData&)), this, SLOT(wkr_onNewStatusData(const UsbDev::StatusData&)));
        QObject::connect( m_wkr, SIGNAL(newFrameData (const UsbDev::FrameData&)), this, SLOT(wkr_onNewFrameData(const UsbDev::FrameData&)));
        QObject::connect( m_wkr, SIGNAL(videoStatusChanged(bool)), this, SLOT(wkr_onVideoStatusChanged(bool)));
        QObject::connect( m_wkr, SIGNAL(updateInfo(QString)), this, SIGNAL(updateInfo(QString)));
        QObject::connect( m_wkr, SIGNAL(updateIOInfo(QString)), this, SIGNAL(updateIOInfo(QString)));
        QObject::connect( m_wkr, SIGNAL(updateRefreshInfo(QString)), this, SIGNAL(updateRefreshInfo(QString)));
        QObject::connect( m_wkr, SIGNAL(updateRefreshIOInfo(QString)), this, SIGNAL(updateRefreshIOInfo(QString)));
        //这里导致CPU占用高,注释掉之后,CPU占用为0.
        if ( m_status_tmr != Q_NULLPTR ) { // connect the trigger function
           QObject::connect( m_status_tmr, SIGNAL(timeout()), m_wkr, SLOT(status_trigger_di()), Qt::DirectConnection );
        }

        if ( m_video_tmr != Q_NULLPTR ) { // connect the trigger function
           QObject::connect( m_video_tmr, SIGNAL(timeout()), m_wkr, SLOT(video_trigger_di()), Qt::DirectConnection );
        }

    } else {    // delete the worker
        if ( m_wkr != Q_NULLPTR ) {
//            QxPack::IcRmtObjSigBlocker::blockSignals( m_wkr, true );     //影响退出.
            m_wkr->deleteLater();
            m_wkr = Q_NULLPTR;
        }
    }
}

// ============================================================================
// initialize   在devCtl构造函数里面init
// ============================================================================
void  DevCtlPriv :: init( bool req_sync )
{

    this->ensureTimer ( true );  //init timer .
    this->ensureWorker( true );   //here create instance of worker and connect signals and slots include timer connect to triggerdi.
    if ( m_wkr != Q_NULLPTR ) {
        QMetaObject::invokeMethod(
            m_wkr, "init", ( req_sync ? Qt::BlockingQueuedConnection : Qt::QueuedConnection ),  //init dev_wkr 读取profile. config.
            Q_ARG( bool, req_sync ? false : true  ) // req_sync(true) means need not to emit signal about workstatus and profile.
        );
        if ( req_sync ) { // added since 0.3.1
            QMetaObject::invokeMethod(
                m_wkr, "forceReadProfile", Qt::BlockingQueuedConnection,
                Q_ARG( void*, reinterpret_cast<void*>(& m_profile ))
            );
        }
    }

    m_is_inited = true;
}

// ============================================================================
// slot: handle the work status changed
// ============================================================================
void  DevCtlPriv :: wkr_onWorkStatusChanged( int s )
{
    if ( m_wkr_status != s ) {
        m_wkr_status = static_cast<DevCtl::WorkStatus>( s );
        emit this->workStatusChanged( s );
    }
}

// ============================================================================
// slot: handle the video status changed
// ============================================================================
void  DevCtlPriv :: wkr_onVideoStatusChanged( bool curr_st )
{
    if ( ! curr_st ) {
        m_frame_data_emit_cntr = 0;
        m_frame_data_queue.clear();
    }
}

// ============================================================================
// slot: handle the new status data
// ============================================================================
void  DevCtlPriv :: wkr_onNewStatusData( const UsbDev::StatusData &sd )
{
    m_status_data_queue.forceEnqueue( sd );
    this->emitNewStatusData();
}

// ============================================================================
// slot: handle the new frame data
// ============================================================================
void  DevCtlPriv :: wkr_onNewFrameData( const UsbDev::FrameData &fd )
{
    m_frame_data_queue.forceEnqueue( fd );
    this->emitNewFrameData();
}

// ============================================================================
// slot: handle the new profile
// ============================================================================
void  DevCtlPriv :: wkr_onNewProfile( const UsbDev::Profile &pf )
{
    m_profile = pf;
    emit this->newProfile();
}

// ============================================================================
// slot: handle the new profile
// ============================================================================
void  DevCtlPriv :: wkr_onNewConfig( const UsbDev::Config &cfg )
{
    m_config = cfg;
    emit this->newConfig();
}

// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// [ static ] create private object
// ============================================================================
static DevCtlPriv *  gCreateDevCtlPriv( DevCtl *dev, quint32 vid_pid, quint32 cfg_id )
{
    DevCtlPriv *priv = usbdev_new( DevCtlPriv, vid_pid, cfg_id );

    // ------------------------------------------------------------------------
    // here need not to use queued connection due to the worker is in another
    // thread.
    // ------------------------------------------------------------------------
    QObject::connect( priv, SIGNAL(newFrameData()),  dev, SIGNAL(newFrameData())  );
    QObject::connect( priv, SIGNAL(newProfile()),    dev, SIGNAL(newProfile())    );
    QObject::connect( priv, SIGNAL(newConfig()),     dev, SIGNAL(newConfig())    );
    QObject::connect( priv, SIGNAL(newStatusData()), dev, SIGNAL(newStatusData()) );
    QObject::connect( priv, SIGNAL(workStatusChanged(int)), dev, SIGNAL(workStatusChanged(int)));
    QObject::connect( priv, SIGNAL(updateInfo(QString)), dev, SIGNAL(updateInfo(QString)));
    QObject::connect( priv, SIGNAL(updateIOInfo(QString)), dev, SIGNAL(updateIOInfo(QString)));
    QObject::connect( priv, SIGNAL(updateRefreshInfo(QString)), dev, SIGNAL(updateRefreshInfo(QString)));
    QObject::connect( priv, SIGNAL(updateRefreshIOInfo(QString)), dev, SIGNAL(updateRefreshIOInfo(QString)));
    return priv;
}

// ============================================================================
// ctor
// ============================================================================
DevCtl :: DevCtl( quint32 vid_pid, quint32 cfg_id )
{
    m_obj = gCreateDevCtlPriv( this, vid_pid, cfg_id );
    T_PrivPtr( m_obj )->init( false );
}

// ============================================================================
// ctor( with wait )
// ============================================================================
DevCtl :: DevCtl( quint32 vid_pid, quint32 cfg_id, bool )
{
    m_obj = gCreateDevCtlPriv( this, vid_pid, cfg_id );
    T_PrivPtr( m_obj )->init( true ); // block until the device conntected and read profile

    int wks = 0;
    QMetaObject::invokeMethod(
        T_PrivPtr(m_obj)->wkrPtr(), "forceReadWorkStatus", Qt::BlockingQueuedConnection,
        Q_RETURN_ARG( int, wks )
    );
    T_PrivPtr( m_obj )->workStatus() = static_cast<DevCtl::WorkStatus>( wks );
}

// ============================================================================
// dtor
// ============================================================================
DevCtl :: ~DevCtl( )
{
    this->blockSignals(true);
    T_PrivPtr( m_obj )->blockSignals(true);
    usbdev_delete( m_obj, DevCtlPriv );
}

// ============================================================================
// return current work status
// ============================================================================
DevCtl::WorkStatus  DevCtl :: workStatus() const
{ return T_PrivPtr( m_obj )->workStatus(); }

// ============================================================================
// init the device
// ============================================================================
void  DevCtl :: reinit ( )
{
    if ( T_PrivPtr( m_obj )->isInited()) {
        quint32 vid_pid = T_PrivPtr( m_obj )->vidPid();
        quint32 cfg_id  = T_PrivPtr( m_obj )->cfgId();
        T_PrivPtr( m_obj )->blockSignals(true);
        usbdev_delete( m_obj, DevCtlPriv );
        m_obj = gCreateDevCtlPriv( this, vid_pid, cfg_id );
    }
    T_PrivPtr( m_obj )->init( false );
}

// ============================================================================
// return the profile data
// ============================================================================
UsbDev::Profile  DevCtl :: profile() const
{   return T_PrivPtr( m_obj )->profile(); }


// ============================================================================
// return the config data
// ============================================================================
UsbDev::Config  DevCtl :: config() const
{   return T_PrivPtr( m_obj )->config(); }



// ============================================================================
// take the pending status data
// ============================================================================
UsbDev::StatusData   DevCtl :: takeNextPendingStatusData()
{
    UsbDev::StatusData sd;
    bool ret = T_PrivPtr( m_obj )->takeNextPendingStatusData( sd );
    return ( ret ? sd : UsbDev::StatusData());
}

// ============================================================================
// take the pending frame data
// ============================================================================
UsbDev::FrameData    DevCtl :: takeNextPendingFrameData()
{
    UsbDev::FrameData fd;
    bool ret = T_PrivPtr( m_obj )->takeNextPendingFrameData( fd );
    return ( ret ? fd : UsbDev::FrameData());
}



// ============================================================================
// move the motor
// ============================================================================
void  DevCtl :: moveChinMotors( quint8* sps, qint32* dist,MoveMethod method)
{

    QByteArray ba(512,0);ba.fill(0);
    unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
    ptr[0]=0x5a;
    method==MoveMethod::Relative? ptr[1]=0x50: ptr[1]=0x51;
    memcpy(ptr+2,sps,2);
    memcpy(ptr+4,dist,8);
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
        Q_ARG( QByteArray, ba ),Q_ARG( QString,QString("move Chin motor")),Q_ARG( quint32, 12 )
    );
}


// ============================================================================
// move the motor
// ============================================================================
void   DevCtl :: move5Motors( quint8*  sps, qint32*  dist,MoveMethod method)
{
    QByteArray ba(512,0);ba.fill(0);
    unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
    ptr[0]=0x5a;
    method==MoveMethod::Relative? ptr[1]=0x52: ptr[1]=0x53;
    memcpy(ptr+3,sps,5);
    memcpy(ptr+8,dist,20);
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
        Q_ARG( QByteArray, ba  ),Q_ARG( QString,QString("move five motors")),Q_ARG( quint32, 28 )
                );
}

void DevCtl::sendDynamicData(quint8 totalFrame, quint8 frameNumber, quint32 dataLen, qint32 *posData)
{
    QByteArray ba(512,0);ba.fill(0);
    unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
    ptr[0]=0x5a;ptr[1]=0x54;ptr[2]=totalFrame;ptr[3]=frameNumber;
    memcpy(ptr+4,&dataLen,4);
    memcpy(ptr+8,posData,dataLen-8);
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
        Q_ARG( QByteArray, ba  ),Q_ARG( QString,QString("send dynamic data")),Q_ARG( quint32, dataLen )
                );
}

void DevCtl::startDynamic(quint8 spsX, quint8 spsY, quint8 spsF, quint32 stepTime,quint32 totalStepCount)
{
    QByteArray ba(512,0);ba.fill(0);
    unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
    ptr[0]=0x5a;ptr[1]=0x55;ptr[5]=spsX;ptr[6]=spsY;ptr[7]=spsF;
    memcpy(ptr+8,&stepTime,4);
    memcpy(ptr+12,&totalStepCount,12);
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
        Q_ARG( QByteArray, ba  ),Q_ARG( QString,QString("start dyanmic move")),Q_ARG( quint32, 16 )
                );
}

void DevCtl::stopDyanmic()
{
    QByteArray ba(512,0);ba.fill(0);
    unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
    ptr[0]=0x5a;ptr[1]=0x58;
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
        Q_ARG( QByteArray, ba  ),Q_ARG( QString,QString("stop dynamic move")),Q_ARG( quint32, 4 )
                );
}


// ============================================================================
// reset the motor
// ============================================================================
void   DevCtl :: resetMotor( MotorId mot,quint8 speed )
{
    QByteArray ba(512,0);
    unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
    ptr[0]=0x5a;ptr[1]=0x57;ptr[2]=mot;ptr[3]=speed;
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
        Q_ARG( QByteArray, ba  ),Q_ARG( QString,QString("reset motor")),Q_ARG( quint32, 4 )
    );
}


// ============================================================================
// save the  config
// ============================================================================
void   DevCtl :: saveConfig(Config& cfg )
{
    int frameSize=512-4;
    int totalFrame=ceil(double(cfg.dataLen())/frameSize);
    int fragment=cfg.dataLen()-frameSize*(totalFrame-1);
    unsigned char* dataPtr=(unsigned char*)cfg.dataPtr();

    for(int i=0;i<totalFrame;i++)
    {
        QByteArray ba(512,0);
        unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
        ptr[0]=0x5a;ptr[1]=0xf2;ptr[2]=totalFrame;ptr[3]=i;
        if(i!=totalFrame-1)
        {
            memcpy(ptr+4,dataPtr+frameSize*i,frameSize);
            QMetaObject::invokeMethod(
             T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
             Q_ARG( QByteArray, ba  ),Q_ARG( QString,QString("upload config")),Q_ARG( quint32, 512)
            );
        }

        else
        {
            memcpy(ptr+4,dataPtr+frameSize*i,fragment);
            QMetaObject::invokeMethod(
             T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
             Q_ARG( QByteArray, ba  ),Q_ARG( QString,QString("upload config")),Q_ARG( quint32, fragment+4)
            );
        }
    }
}

// ============================================================================
// sample motor control
// ============================================================================
void   DevCtl :: sampleMotorCtl( SampleMotorCtl_Stage stage, qint32 sps, quint8 acc_flag )
{
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_ControlSampleMotor", Qt::QueuedConnection,
        Q_ARG( int, stage ), Q_ARG( qint32, sps ), Q_ARG( quint8, acc_flag )
    );
}

// ============================================================================
// open the front video
// ============================================================================
void   DevCtl :: setFrontVideo( bool on_off )
{
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), ( on_off ? "cmd_TurnOnVideo" : "cmd_TurnOffVideo" ),
        Qt::QueuedConnection
                );
}

StaticCache* DevCtl::readStaticCache() const
{
    StaticCache* retVal=Q_NULLPTR;
    QMetaObject::invokeMethod(
                T_PrivPtr( m_obj )->wkrPtr(), "cmd_ReadStaticCache", Qt::BlockingQueuedConnection,
                Q_RETURN_ARG( StaticCache*, retVal )
                );
    return retVal;
}

MoveCache *DevCtl::readMoveCache() const
{
    MoveCache* retVal=Q_NULLPTR;
    QMetaObject::invokeMethod(
                T_PrivPtr( m_obj )->wkrPtr(), "cmd_ReadMoveCache", Qt::BlockingQueuedConnection,
                Q_RETURN_ARG( MoveCache*, retVal )
                );
    return retVal;
}

void DevCtl::readConfig()
{
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), ("cmd_ReadConfig" ),Qt::QueuedConnection );
}

void DevCtl::sendBinaryCommand(QByteArray ba,int dataLen)
{
    ba.resize(512);
    QMetaObject::invokeMethod(
     T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
     Q_ARG( QByteArray, ba  ),Q_ARG( QString,QString("binary command")),Q_ARG( quint32, dataLen)
    );
}

// ============================================================================
// set the lamp
// ============================================================================
void   DevCtl :: setLamp( LampId lampId,quint8 lampNumber, quint16 da)
{
    QByteArray ba(512,0);
    unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
    ptr[0]=0x5a;ptr[1]=0x80;ptr[2]=lampId;ptr[3]=lampNumber;
    gPutInt16(&ptr[4],da);
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
        Q_ARG( QByteArray, ba),Q_ARG( QString,QString("set Lamp")),Q_ARG( quint32, 6 )
        );
}

void DevCtl::setWhiteLamp(quint8 r,quint8 g,quint8 b)
{
    QByteArray ba(512,0);
    unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
    ptr[0]=0x5a;ptr[1]=0x81;ptr[2]=r;ptr[3]=g;ptr[4]=b;
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
        Q_ARG( QByteArray, ba),Q_ARG( QString,QString("set white Lamp")),Q_ARG( quint32, 5 )
                );
}

void DevCtl::openShutter(quint16 durationTime, qint32 coord_shutter)
{
    QByteArray ba(512,0);
    unsigned char* ptr=reinterpret_cast<unsigned char*>(ba.data());
    ptr[0]=0x5a;ptr[1]=0x56;
    gPutInt16(&ptr[2],durationTime);
    gPutInt32(&ptr[4],coord_shutter);
    QMetaObject::invokeMethod(
        T_PrivPtr( m_obj )->wkrPtr(), "cmd_GeneralCmd", Qt::QueuedConnection,
        Q_ARG( QByteArray, ba),Q_ARG( QString,QString("open shutter")),Q_ARG( quint32, 8 )
                );
}




// ============================================================================
// EEPRom read/write
// ============================================================================
auto     DevCtl :: readUsbEEPROM (  char *buff_ptr, int size, int eeprom_addr ) -> bool
{
    auto priv = T_PrivPtr(m_obj);
    if ( priv->workStatus() != WorkStatus_S_OK ) { return false; }
    if ( size < 1 ) { return false; }

    bool c_ret = false;
    auto ret = QMetaObject::invokeMethod(
        priv->wkrPtr(), "cmd_readUsbEEPROM", Qt::BlockingQueuedConnection,
        Q_RETURN_ARG( bool,c_ret ), Q_ARG( char*, buff_ptr ),
        Q_ARG( int, size ), Q_ARG( int, eeprom_addr )
    );
    return ( ret && c_ret );
}

auto     DevCtl :: writeUsbEEPROM( const char *buff_ptr, int size, int eeprom_addr ) -> bool
{
    auto priv = T_PrivPtr(m_obj);
    if ( priv->workStatus() != WorkStatus_S_OK ) { return false; }
    if ( size < 1 ) { return false; }

    bool c_ret = false;
    auto ret = QMetaObject::invokeMethod(
        priv->wkrPtr(), "cmd_writeUsbEEPROM", Qt::BlockingQueuedConnection,
        Q_RETURN_ARG( bool,c_ret ), Q_ARG( const char*, buff_ptr ),
        Q_ARG( int, size ), Q_ARG( int, eeprom_addr )
    );
    return ( ret && c_ret );
}


// ////////////////////////////////////////////////////////////////////////////
// static functions
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// create the device
// ============================================================================
UsbDev::DevCtl* DevCtl :: createInstance( quint32 vid_pid, quint32 cfg_id )
{
    if(logger==NULL)
    {
        logger = spdlog::rotating_logger_mt("logger", "logs/UsbDev_logger.txt", 1024*1024*30, 30);
        spdlog::flush_on(spdlog::level::info);
    }
    logger->info("create instance.");
    return usbdev_new_qobj( UsbDev::DevCtl, vid_pid, cfg_id );
}

// ============================================================================
// since 0.2.1, create the device in sync, ensure device is created OK!
// ============================================================================
UsbDev::DevCtl*   DevCtl :: createInstanceSync( quint32 vid_pid, quint32 cfg_id )
{
    if(logger==NULL)
    {
        logger = spdlog::rotating_logger_mt("logger", "logs/UsbDev_logger.txt", 1024*1024*30, 30);
        spdlog::flush_on(spdlog::level::info);
    }
    logger->info("createInstanceSync.");
    return usbdev_new_qobj( UsbDev::DevCtl, vid_pid, cfg_id, true ); }
}




#include "usbdev_devctl.moc"
#endif
