#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UsbViewer/UsbViewerQt.h"
#include <QDebug>
#include <QSettings>
//#include <spdlog/spdlog.h>
//#include <spdlog/sinks/rotating_file_sink.h>
#include <usbdev/main/usbdev_statusdata.hxx>
#include <QImage>
#include <QPainter>
#include <math.h>
#include <QTime>
#include <thread>
#include <future>
#include <QFuture>
#include <QtConcurrent>
#include <array>
#include <QSharedPointer>
#include <QStylePainter>
#include "RbTableHeaderView.h"
#include <QDesktopWidget>
#include <QtGui>
#include <QFileDialog>
#include "windows.h"
#include <QDir>
#include<QElapsedTimer>
#include <QDateTime>
#pragma execution_character_set("utf-8")

//#define MOVERETRY 3;

#define SET_BLOCKING_VALUE(obj,value)       \
{                                           \
    obj->blockSignals(true);                \
    obj->setValue(value);                   \
    obj->blockSignals(false);               \
}                                           \

//static std::shared_ptr<spdlog::logger> logger=NULL;
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    if(logger==NULL) logger = spdlog::rotating_logger_mt("logger", "logs/perimeterConfig.txt", 1024*1024, 30);
//    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);                    // 禁止最大化按钮
//    setWindowFlag(Qt::WindowMaximizeButtonHint,false);
//    m_width=width();m_height=height();
//    setFixedSize(m_width,m_height);                                  // 禁止拖动窗口大小
//    setBaseSize(GetSystemMetrics(SM_CXFULLSCREEN),GetSystemMetrics(SM_CYFULLSCREEN));
//    setMaximumSize(1440,843);
//    QSettings *configIni = new QSettings("para.ini", QSettings::IniFormat);
    VID=m_settings.m_VID;
    PID=m_settings.m_PID;
    m_reconnTimer=new QTimer();
    m_takingPhotoTimer=new QTimer();

    connect(m_reconnTimer,&QTimer::timeout,[&](){
        ui->label_connectionStatus->setText("连接断开");
    });
    connect(m_takingPhotoTimer,&QTimer::timeout,[&](){
        m_takePhoto=true;
    });
    init();

//    m_gl_image=new GL_Image(this);
//    m_gl_image->setMinimumHeight(240);
//    m_gl_image->setMinimumWidth(320);
//    ui->horizontalLayout_GL->addWidget(m_gl_image);
}

void MainWindow::on_action_chooseDevice_triggered()
{

    auto* dialog=new UsbViewerQt(this);
    dialog->setModal(true);
    if(dialog->exec()==QDialog::Accepted)
    {
        VID=dialog->VID;
        PID=dialog->PID;
        if(m_devCtl!=NULL)
        {
            uninitDevCtl();
        }
        initDevCtl();
    }
}


void MainWindow::init()
{
    initData();
    initTable();
    initDevCtl();
    initWidget();

}

void MainWindow::initDevCtl()
{
    quint32 vid_pid=VID.toInt(nullptr,16)<<16|PID.toInt(nullptr,16);
    m_devCtl=UsbDev::DevCtl::createInstance(vid_pid);
    ui->label_VID->setText(VID);
    ui->label_PID->setText(PID);
    connect(m_devCtl,&UsbDev::DevCtl::workStatusChanged,this,&MainWindow::refreshConnectionStatus);
    connect(m_devCtl,&UsbDev::DevCtl::updateInfo,this,&MainWindow::showDevInfo);
    connect(m_devCtl,&UsbDev::DevCtl::newStatusData,this,&MainWindow::refreshStatus);
    connect(m_devCtl,&UsbDev::DevCtl::newFrameData,this,&MainWindow::refreshVideo);
    connect(m_devCtl,&UsbDev::DevCtl::newProfile,this,&MainWindow::updateProfile);
    connect(m_devCtl,&UsbDev::DevCtl::newConfig,this,&MainWindow::updateConfig);
    ui->checkBox_IO->setChecked(m_settings.m_updateIOInfo);
    ui->checkBox_startRefreshInfo->setChecked(m_settings.m_updateRefreshIOInfo);
    ui->checkBox_RefreshIO->setChecked(m_settings.m_updateRefreshIOInfo);

}


void MainWindow::initData()
{
    QString configPath=m_settings.localConfigPath;
    QString dataPath=m_settings.localDataPath;
    readLocalConfig(configPath);
    readLocalData(dataPath);
}



void MainWindow::initTable()
{
    m_colorPosTableModel=new TableModel();
    m_colorPosTableModel->m_column=1;
    m_colorPosTableModel->m_row=5;
    m_colorPosTableModel->m_hozHeader<<"步数";
//    m_colorPosTableModel->m_vertHeader<<"全透";
    m_colorPosTableModel->m_modelData=m_config.switchColorMotorPosPtr();
    ui->tableView_colorSlotPos->setModel(m_colorPosTableModel);
    ui->tableView_colorSlotPos->setCornerName("颜色");
    ui->tableView_colorSlotPos->verticalHeader()->setVisible(true);
    m_colorPosTableModel->m_vertHeader<<"1"<<"2"<<"3"<<"4"<<"5";

    m_spotPosTableModel=new TableModel();
    m_spotPosTableModel->m_column=1;
    m_spotPosTableModel->m_row=6;
    m_spotPosTableModel->m_hozHeader<<"步数";
    m_spotPosTableModel->m_vertHeader<<"1"<<"2"<<"3"<<"4"<<"5"<<"6";

    m_spotPosTableModel->m_modelData=m_config.switchLightSpotMotorPosPtr();
    ui->tableView_spotSlotPos->setModel(m_spotPosTableModel);
    ui->tableView_spotSlotPos->setCornerName("光斑");
    ui->tableView_spotSlotPos->verticalHeader()->setVisible(true);

    auto tableData=m_localTableData.m_xyDistTableData;
    m_xyDistTableModel=new TableModel();
    m_xyDistTableModel->m_column=tableData.m_column;
    m_xyDistTableModel->m_row=tableData.m_row;
    m_xyDistTableModel->m_hozHeader<<"X"<<"Y";
    m_xyDistTableModel->m_modelData=tableData.m_data;
    ui->tableView_XYDistTable->setModel(m_xyDistTableModel);
    ui->tableView_XYDistTable->verticalHeader()->setVisible(false);

    tableData=m_localTableData.m_focalLengthMotorPosMappingData;
    m_spotDistFocalPosModel=new TableModel();
    m_spotDistFocalPosModel->m_column=tableData.m_column;
    m_spotDistFocalPosModel->m_row=tableData.m_row;
    m_spotDistFocalPosModel->m_hozHeader<<"光斑1"<<"光斑2"<<"光斑3"<<"光斑4"<<"光斑5"<<"光斑6";
    for(int i=80;i<=320;i+=10){m_spotDistFocalPosModel->m_vertHeader<<QString::number(i);}
//    m_spotDistFocalPosModel->m_modelData=(int*)m_config.focalLengthMotorPosMappingPtr();
    m_spotDistFocalPosModel->m_modelData=tableData.m_data;
    ui->tableView_focalPosTable->setModel(m_spotDistFocalPosModel);
    ui->tableView_focalPosTable->setCornerName("距离");
    ui->tableView_focalPosTable->verticalHeader()->setVisible(true);

    m_dbColorSpotPosTableModel=new TableModel();
    m_dbColorSpotPosTableModel->m_column=2;
    m_dbColorSpotPosTableModel->m_row=52;
    m_dbColorSpotPosTableModel->m_hozHeader<<"颜色步"<<"尺寸步";
    for(int i=0;i<=51;i++){m_dbColorSpotPosTableModel->m_vertHeader<<QString::number(i);}
    m_dbColorSpotPosTableModel->m_modelData=(int*)m_config.DbPosMappingPtr();
    ui->tableView_dbColorSpotPosTable->setModel(m_dbColorSpotPosTableModel);
    ui->tableView_dbColorSpotPosTable->setCornerName("DB");
    ui->tableView_dbColorSpotPosTable->verticalHeader()->setVisible(true);


    tableData=m_localTableData.m_dbAngleDampingTableData;
    m_dbAngleDampingTableModel=new TableModel();
    m_dbAngleDampingTableModel->m_column=tableData.m_column;
    m_dbAngleDampingTableModel->m_row=tableData.m_row;
    m_dbAngleDampingTableModel->m_hozHeader<<"DB衰减";
    for(int i=0;i<=90;i+=2){m_dbAngleDampingTableModel->m_vertHeader<<QString::number(i);}
    m_dbAngleDampingTableModel->m_modelData=tableData.m_data;
    ui->tableView_dbAngleDampingTable->setModel(m_dbAngleDampingTableModel);
    ui->tableView_dbAngleDampingTable->setCornerName("离心度");
    ui->tableView_dbAngleDampingTable->verticalHeader()->setVisible(true);

    tableData=m_localTableData.m_dynamicLenAndTimeData;
    m_dynamicLenAndTimeModel=new TableModel();
    m_dynamicLenAndTimeModel->m_column=tableData.m_column;
    m_dynamicLenAndTimeModel->m_row=tableData.m_row;
    m_dynamicLenAndTimeModel->m_hozHeader<<"步长"<<"时间";
    for(int i=1;i<=9;i+=1){m_dynamicLenAndTimeModel->m_vertHeader<<QString::number(i);}
    m_dynamicLenAndTimeModel->m_modelData=tableData.m_data;
    ui->tableView_dynamicSteplenAndTime->setModel(m_dynamicLenAndTimeModel);
    ui->tableView_dynamicSteplenAndTime->setCornerName("速度等级");
    ui->tableView_dynamicSteplenAndTime->verticalHeader()->setVisible(true);


    ui->tableView_mainMotorPosTable->setData(m_localTableData.m_mainPosTableData.m_data);
    ui->tableView_secondaryPosTable->setData(m_localTableData.m_secondaryPosTableData.m_data);
}




void MainWindow::uninitDevCtl()
{
    m_reconnTimer->stop();
    ui->label_connectionStatus->setText("未连接");
    disconnect(m_devCtl,&UsbDev::DevCtl::workStatusChanged,this,&MainWindow::refreshConnectionStatus);
    disconnect(m_devCtl,&UsbDev::DevCtl::updateInfo,this,&MainWindow::showDevInfo);
    disconnect(m_devCtl,&UsbDev::DevCtl::newStatusData,this,&MainWindow::refreshStatus);
    disconnect(m_devCtl,&UsbDev::DevCtl::newFrameData,this,&MainWindow::refreshVideo);
    disconnect(m_devCtl,&UsbDev::DevCtl::newProfile,this,&MainWindow::updateProfile);
    disconnect(m_devCtl,&UsbDev::DevCtl::newConfig,this,&MainWindow::updateConfig);
    ui->checkBox_IO->setChecked(Qt::CheckState::Unchecked);
    ui->checkBox_startRefreshInfo->setChecked(Qt::CheckState::Unchecked);
    ui->checkBox_RefreshIO->setChecked(Qt::CheckState::Unchecked);

    delete m_devCtl;
}

void MainWindow::initWidget()
{
    for(auto &v:m_settings.m_colorToSlot)
    {
//        if(v.first==text)
//        {
//            ui->spinBox_colorSlot->setValue(v.second);
//        }
        ui->comboBox_color->addItem(v.first);
    }
    for(auto &v:m_settings.m_spotSizeToSlot)
    {
//        if(v.first==text)
//        {
//            ui->spinBox_colorSlot->setValue(v.second);
//        }
        ui->comboBox_spotSize->addItem(v.first);
    }
    ui->comboBox_color->setCurrentIndex(0);
    ui->comboBox_spotSize->setCurrentIndex(0);
    on_spinBox_colorSlot_valueChanged(ui->spinBox_colorSlot->value());
    on_spinBox_spotSlot_valueChanged(ui->spinBox_spotSlot->value());
    on_spinBox_DbSetting_valueChanged(0);
    fillXYMotorAndFocalInfoByXYCoord();
}

int MainWindow::interpolation(int value[], QPointF loc)
{
    double secondVal[2];
    secondVal[0]=value[0]+(value[1]-value[0])*(loc.x()/6.0);
    secondVal[1]=value[2]+(value[3]-value[2])*(loc.x()/6.0);
    int ret=secondVal[0]+(secondVal[1]-secondVal[0])*(loc.y()/6.0);
    return ret;
}


int MainWindow::getFocusMotorPosByDist(int focalDist,int spotSlot)
{
    if(m_config.isEmpty()) {return 0;}
    auto map = m_localTableData.m_focalLengthMotorPosMappingData;
    int indexDist= floor(focalDist/10)-8;
    int pos1=map(indexDist,spotSlot-1);
    int pos2=map(indexDist+1,spotSlot-1);
    int focalMotorPos=pos1+(pos2-pos1)*(focalDist%10)/10+m_config.focalMotorPosCorrectionRef();
    return focalMotorPos;
}

void MainWindow::initConfigUI()
{
    ui->comboBox_lightSelect_1->currentIndexChanged(0);
    ui->comboBox_lightSelect_2->currentIndexChanged(0);
    ui->spinBox_lightR->setValue(m_config.whiteBackgroundLampDAPtr()[0]);
    ui->spinBox_lightG->setValue(m_config.whiteBackgroundLampDAPtr()[1]);
    ui->spinBox_lightB->setValue(m_config.whiteBackgroundLampDAPtr()[2]);
}


void MainWindow::refreshConfigUI()
{
    ui->spinBox_shutterOpenPos->setValue(m_config.shutterOpenPosRef());
    ui->lineEdit_deviceSerialNo->setText(QString(m_config.deviceIDRef()));
    ui->lineEdit_centralLightDA->setText(QString::number(m_config.centerFixationLampDARef()));
    ui->lineEdit_bigDiamond1DA->setText(QString::number(m_config.bigDiamondfixationLampDAPtr()[0]));
    ui->lineEdit_bigDiamond2DA->setText(QString::number(m_config.bigDiamondfixationLampDAPtr()[1]));
    ui->lineEdit_bigDiamond3DA->setText(QString::number(m_config.bigDiamondfixationLampDAPtr()[2]));
    ui->lineEdit_bigDiamond4DA->setText(QString::number(m_config.bigDiamondfixationLampDAPtr()[3]));
    ui->lineEdit_smallDiamond1DA->setText(QString::number(m_config.smallDiamondFixationLampDAPtr()[0]));
    ui->lineEdit_smallDiamond2DA->setText(QString::number(m_config.smallDiamondFixationLampDAPtr()[1]));
    ui->lineEdit_smallDiamond3DA->setText(QString::number(m_config.smallDiamondFixationLampDAPtr()[2]));
    ui->lineEdit_smallDiamond4DA->setText(QString::number(m_config.smallDiamondFixationLampDAPtr()[3]));
    ui->lineEdit_yellowBackGroundLampDa->setText(QString::number(m_config.yellowBackgroundLampDARef()));
    ui->lineEdit_whiteBackGroundLampR->setText(QString::number(m_config.whiteBackgroundLampDAPtr()[0]));
    ui->lineEdit_whiteBackGroundLampG->setText(QString::number(m_config.whiteBackgroundLampDAPtr()[1]));
    ui->lineEdit_whiteBackGroundLampB->setText(QString::number(m_config.whiteBackgroundLampDAPtr()[2]));
    ui->lineEdit_centerInfraredLampDA->setText(QString::number(m_config.centerInfraredLampDARef()));
    ui->lineEdit_borderLampDA->setText(QString::number(m_config.borderInfraredLampDARef()));
    ui->lineEdit_eyeGlassLampDa->setText(QString::number(m_config.eyeglassFrameLampDARef()));
    ui->lineEdit_whiteLampEnvLightAlarm->setText(QString::number(m_config.environmentAlarmLightDAPtr()[0]));
    ui->lineEdit_yellowLampEnvLightAlarm->setText(QString::number(m_config.environmentAlarmLightDAPtr()[1]));
    ui->lineEdit_whiteLampPupilGray->setText(QString::number(m_config.pupilGreyThresholdDAPtr()[0]));
    ui->lineEdit_yellowLampPupilGray->setText(QString::number(m_config.pupilGreyThresholdDAPtr()[1]));
    ui->lineEdit_focusUnite->setText(QString::number(m_config.focusPosForSpotAndColorChangeRef()));
    ui->lineEdit_shutterOpen->setText(QString::number(m_config.shutterOpenPosRef()));
    ui->lineEdit_centerX->setText(QString::number(m_config.mainTableCenterXRef()));
    ui->lineEdit_centerY->setText(QString::number(m_config.mainTableCenterYRef()));
    ui->lineEdit_secondaryCenterX->setText(QString::number(m_config.secondaryTableCenterXRef()));
    ui->lineEdit_secondaryCenterY->setText(QString::number(m_config.secondaryTableCenterYRef()));
    ui->lineEdit_focalMotorPosCorrection->setText(QString::number(m_config.focalMotorPosCorrectionRef()));

//    ui->lineEdit_castLight->setText(QString::number(m_config.castLightADPresetRef()));
    ui->lineEdit_castLightCorrectionDB->setText(QString::number(m_config.DBForLightCorrectionRef()));
    ui->lineEdit_castLightCorrenctionSensorDA->setText(QString::number(m_config.castLightSensorDAForLightCorrectionRef()));
    ui->lineEdit_lightCorrectionFocus->setText(QString::number(m_config.focalLengthMotorPosForLightCorrectionRef()));
    ui->lineEdit_lightCorrectionX->setText(QString::number(m_config.xMotorPosForLightCorrectionRef()));
    ui->lineEdit_lightCorrectionY->setText(QString::number(m_config.yMotorPosForLightCorrectionRef()));

//    ui->tableView_dbColorSpotPosTable->viewport()->update();
//    ui->tableView_speedStepTimeTable->viewport()->update();
}

void MainWindow::refreshConfigDataByUI()
{
    bool ok;
//    m_config.deviceIDRef()=ui->lineEdit_deviceSerialNo->text();                              //if(!ok) return;
    memset(m_config.deviceIDRef(),0,32);
    auto ID=ui->lineEdit_deviceSerialNo->text().trimmed().toStdString();
    memcpy(m_config.deviceIDRef(),ID.data(),ID.length());
    m_config.centerFixationLampDARef()=ui->lineEdit_centralLightDA->text().toInt(&ok);                  //if(!ok) return;
    m_config.bigDiamondfixationLampDAPtr()[0]=ui->lineEdit_bigDiamond1DA->text().toShort(&ok);            //if(!ok) return;
    m_config.bigDiamondfixationLampDAPtr()[1]=ui->lineEdit_bigDiamond2DA->text().toShort(&ok);            //if(!ok) return;
    m_config.bigDiamondfixationLampDAPtr()[2]=ui->lineEdit_bigDiamond3DA->text().toShort(&ok);            //if(!ok) return;
    m_config.bigDiamondfixationLampDAPtr()[3]=ui->lineEdit_bigDiamond4DA->text().toShort(&ok);            //if(!ok) return;
    m_config.smallDiamondFixationLampDAPtr()[0]=ui->lineEdit_smallDiamond1DA->text().toShort(&ok);        //if(!ok) return;
    m_config.smallDiamondFixationLampDAPtr()[1]=ui->lineEdit_smallDiamond2DA->text().toShort(&ok);        //if(!ok) return;
    m_config.smallDiamondFixationLampDAPtr()[2]=ui->lineEdit_smallDiamond3DA->text().toShort(&ok);        //if(!ok) return;
    m_config.smallDiamondFixationLampDAPtr()[3]=ui->lineEdit_smallDiamond4DA->text().toShort(&ok);        //if(!ok) return;
    m_config.yellowBackgroundLampDARef()=ui->lineEdit_yellowBackGroundLampDa->text().toShort(&ok);        //if(!ok) return;
    m_config.whiteBackgroundLampDAPtr()[0]=ui->lineEdit_whiteBackGroundLampR->text().toShort(&ok);        //if(!ok) return;
    m_config.whiteBackgroundLampDAPtr()[1]=ui->lineEdit_whiteBackGroundLampG->text().toShort(&ok);        //if(!ok) return;
    m_config.whiteBackgroundLampDAPtr()[2]=ui->lineEdit_whiteBackGroundLampB->text().toShort(&ok);        //if(!ok) return;
    m_config.centerInfraredLampDARef()=ui->lineEdit_centerInfraredLampDA->text().toShort(&ok);            //if(!ok) return;
    m_config.borderInfraredLampDARef()=ui->lineEdit_borderLampDA->text().toShort(&ok);                    //if(!ok) return;
    m_config.eyeglassFrameLampDARef()=ui->lineEdit_eyeGlassLampDa->text().toShort(&ok);                   //if(!ok) return;
    m_config.environmentAlarmLightDAPtr()[0]=ui->lineEdit_whiteLampEnvLightAlarm->text().toShort(&ok);
    m_config.environmentAlarmLightDAPtr()[1]=ui->lineEdit_yellowLampEnvLightAlarm->text().toShort(&ok);
    m_config.pupilGreyThresholdDAPtr()[0]=ui->lineEdit_whiteLampPupilGray->text().toShort(&ok);
    m_config.pupilGreyThresholdDAPtr()[1]=ui->lineEdit_yellowLampPupilGray->text().toShort(&ok);
    m_config.focusPosForSpotAndColorChangeRef()=ui->lineEdit_focusUnite->text().toInt(&ok);
    m_config.shutterOpenPosRef()=ui->lineEdit_shutterOpen->text().toInt(&ok);
    m_config.mainTableCenterXRef()=ui->lineEdit_centerX->text().toInt(&ok);
    m_config.mainTableCenterYRef()=ui->lineEdit_centerY->text().toInt(&ok);
    m_config.secondaryTableCenterXRef()=ui->lineEdit_secondaryCenterX->text().toInt(&ok);
    m_config.secondaryTableCenterYRef()=ui->lineEdit_secondaryCenterY->text().toInt(&ok);
    m_config.focalMotorPosCorrectionRef()=ui->lineEdit_focalMotorPosCorrection->text().toInt(&ok);
    m_config.DBForLightCorrectionRef()=ui->lineEdit_castLightCorrectionDB->text().toInt(&ok);
    m_config.castLightSensorDAForLightCorrectionRef()=ui->lineEdit_castLightCorrenctionSensorDA->text().toInt(&ok);
    m_config.focalLengthMotorPosForLightCorrectionRef()=ui->lineEdit_lightCorrectionFocus->text().toInt(&ok);
    m_config.xMotorPosForLightCorrectionRef()=ui->lineEdit_lightCorrectionX->text().toInt(&ok);
    m_config.yMotorPosForLightCorrectionRef()=ui->lineEdit_lightCorrectionY->text().toInt(&ok);
    quint32 crc=calcCrc((quint8*)m_config.dataPtr()+4, m_config.dataLen()-4);
    m_config.crcVeryficationRef()=crc;
    qDebug()<<QString("文件校验码:")+QString::number(m_config.crcVeryficationRef());
}



void MainWindow::waitMotorStop(QVector<UsbDev::DevCtl::MotorId> motorIDs)
{

    auto getBusy=[&]()->bool
    {
        for(auto& motorId:motorIDs)
        {
            if(m_statusData.isMotorBusy(motorId))
            {
                return true;
            }
        }
        return false;
    };
    QElapsedTimer mstimer;
    mstimer.restart();//必须先等一会儿刷新状态
    do
    {
        QCoreApplication::processEvents();
    }while(getBusy()||(mstimer.nsecsElapsed()/1000000<500)); //500ms
    //    while(getBusy()){QCoreApplication::processEvents();}
}

void MainWindow::waitForSomeTime(int time)
{
    QElapsedTimer mstimer;
    mstimer.restart();
    do
    {
        QApplication::processEvents();
    }while(mstimer.elapsed()<time);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::refreshStatus()
{
    using MotorId=UsbDev::DevCtl::MotorId;
//    spdlog::info("refreshStatus");
    m_statusData=m_devCtl->takeNextPendingStatusData();

    static bool firstStatus=true;
    bool stat=m_statusData.cameraStatus();
    if(stat&&firstStatus)
    {
        m_devCtl->setFrontVideo(false);
    }
    firstStatus=false;
    m_reconnTimer->start(1000);
    ui->label_connectionStatus->setText("保持连接");
    ui->label_stateX->setText(m_statusData.isMotorBusy(MotorId::MotorId_X)?"忙":"闲");
    ui->label_stateY->setText(m_statusData.isMotorBusy(MotorId::MotorId_Y)?"忙":"闲");
    ui->label_stateFocus->setText(m_statusData.isMotorBusy(MotorId::MotorId_Focus)?"忙":"闲");
    ui->label_stateColor->setText(m_statusData.isMotorBusy(MotorId::MotorId_Color)?"忙":"闲");
    ui->label_stateSpot->setText(m_statusData.isMotorBusy(MotorId::MotorId_Light_Spot)?"忙":"闲");
    ui->label_stateShutter->setText(m_statusData.isMotorBusy(MotorId::MotorId_Shutter)?"忙":"闲");
    ui->label_stateChinHoz->setText(m_statusData.isMotorBusy(MotorId::MotorId_Chin_Hoz)?"忙":"闲");
    ui->label_stateChinVert->setText(m_statusData.isMotorBusy(MotorId::MotorId_Chin_Vert)?"忙":"闲");

    ui->label_castLightDA->setText(QString::number(m_statusData.castLightSensorDA()));
    ui->label_environmentDA->setText(QString::number(m_statusData.envLightSensorDA()));

    ui->label_cameraStatus->setText(m_statusData.cameraStatus()?"开启":"关闭");
    QString cacheStr;
    if(!(m_statusData.cacheNormalFlag()||m_statusData.cacheNormalFlag())) cacheStr="都不可用";
    else if(m_statusData.cacheNormalFlag()&&(!m_statusData.cacheNormalFlag())) cacheStr="静态可用";
    else if((!m_statusData.cacheNormalFlag())&&m_statusData.cacheNormalFlag()) cacheStr="移动可用";
    else if(m_statusData.cacheNormalFlag()&&m_statusData.cacheNormalFlag()) cacheStr="都可用";
    ui->label_cacheStatus->setText(cacheStr);
    ui->label_answerpadStatus->setText(m_statusData.answerpadStatus()?"按下":"松开");
    ui->label_eyeglassStatus->setText(m_statusData.eyeglassStatus()?"升起":"放下");
    ui->label_status_serialNo->setText(QString::number(m_statusData.serialNO()));

    ui->label_posX->setText(QString::number(m_statusData.motorPosition(MotorId::MotorId_X)));
    ui->label_posY->setText(QString::number(m_statusData.motorPosition(MotorId::MotorId_Y)));
    ui->label_posFocus->setText(QString::number(m_statusData.motorPosition(MotorId::MotorId_Focus)));
    ui->label_posColor->setText(QString::number(m_statusData.motorPosition(MotorId::MotorId_Color)));
    ui->label_posSpot->setText(QString::number(m_statusData.motorPosition(MotorId::MotorId_Light_Spot)));
    ui->label_posShutter->setText(QString::number(m_statusData.motorPosition(MotorId::MotorId_Shutter)));
    ui->label_posChinHoz->setText(QString::number(m_statusData.motorPosition(MotorId::MotorId_Chin_Hoz)));
    ui->label_posChinVert->setText(QString::number(m_statusData.motorPosition(MotorId::MotorId_Chin_Vert)));

    static bool moveCommandSend=false;
    if(m_statusData.moveStatus()==true)
    {
        moveCommandSend=true;
    }else
    {
        if(moveCommandSend)
        {
            m_devCtl->openShutter(0,ui->lineEdit_shutterOpen->text().toInt());
            moveCommandSend=false;
        }
    }

}


void MainWindow::refreshVideo()
{
//    if(m_videoTimer==NULL)
//    {
//        m_videoTimer=new QTimer;
//        m_videoTimer->setInterval(1000);
//        m_videoTimer->start();
//        connect(m_videoTimer,&QTimer::timeout,[&](){m_videoCount++;});
//    }
    QSize size;
    m_profile.isEmpty()?size={0,0}:size=m_profile.videoSize();
    int dataSize=size.width()*size.height();
    if(pixData==NULL){pixData=new quint8[dataSize];}
    m_frameData=m_devCtl->takeNextPendingFrameData();
//    memcpy(pixData+20,m_frameData.rawData().data(),dataSize-20);
//    memset(pixData,pixData[20],20);
//    QImage image=QImage(pixData,size.width(),size.height(),QImage::Format::Format_Grayscale8);

//    auto pixData=(quint8*)(m_frameData.rawData().data());
    auto pixData=reinterpret_cast<quint8*>(m_frameData.rawData().data());
    QImage image=QImage(pixData,size.width(),size.height(),QImage::Format::Format_Grayscale8);
    image=image.convertToFormat(QImage::Format_RGBA8888);
    if(m_takePhoto)
    {
        auto filePath=QString("./savePics/")+QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_MM_ss_zzz")+".bmp";
        image.save(filePath);
    }
    m_takePhoto=false;
    QPainter painter(&image);
    painter.setPen(QPen{Qt::red,1});
    painter.drawLine(QPoint{0,size.height()/2},QPoint{size.width(),size.height()/2});
    painter.drawLine(QPoint{size.width()/2,0},QPoint{size.width()/2,size.height()});



//    QImage image=QImage(pixData,size.width(),size.height(),QImage::Format::Format_Grayscale8);
//    if(m_videoCount>0)
//    {
//        m_videoCount--;
//        QString fileName;
//        fileName="./image/"+QString::number(m_frameData.timeStamp())+".bmp";
//        image.save(fileName);
//    }
//    QPainter painter(m_gl_image);
//    QPixmap pix;
//    pix.convertFromImage(m_image);
//    painter.drawPixmap(0,0,pix.width(), pix.height(),pix);
//    update();
//    auto str=QString("frame time stamp:")+QString::number(m_frameData.timeStamp());
//    emit updateRefreshInfo(str);
    ui->video->setImageData(image);
}

void MainWindow::refreshConnectionStatus(int status)
{
    switch (status)
    {
        case UsbDev::DevCtl::WorkStatus::WorkStatus_E_UnExpected:ui->label_connectionStatus->setText("连接异常");break;
        case UsbDev::DevCtl::WorkStatus::WorkStatus_S_ConnectToDev:ui->label_connectionStatus->setText("正在连接");break;
        case UsbDev::DevCtl::WorkStatus::WorkStatus_S_Disconnected:ui->label_connectionStatus->setText("连接断开");break;
        case UsbDev::DevCtl::WorkStatus::WorkStatus_S_OK:ui->label_connectionStatus->setText("连接正常");m_devCtl->readProfile();break;
    }
}

//TODO
void MainWindow::updateProfile()
{
    using x=UsbDev::DevCtl;
    showDevInfo("Profile Got.");
    m_profile=m_devCtl->profile();
    ui->label_xMotorRange->setText(QString("%1-%2").arg(QString::number(m_profile.motorRange(x::MotorId_X).first)).arg(QString::number(m_profile.motorRange(x::MotorId_X).second)));
    ui->label_yMotorRange->setText(QString("%1-%2").arg(QString::number(m_profile.motorRange(x::MotorId_Y).first)).arg(QString::number(m_profile.motorRange(x::MotorId_Y).second)));
    ui->label_spotMotorRange->setText(QString("%1-%2").arg(QString::number(m_profile.motorRange(x::MotorId_Light_Spot).first)).arg(QString::number(m_profile.motorRange(x::MotorId_Light_Spot).second)));
    ui->label_shutterMotorRange->setText(QString("%1-%2").arg(QString::number(m_profile.motorRange(x::MotorId_Shutter).first)).arg(QString::number(m_profile.motorRange(x::MotorId_Shutter).second)));
    ui->label_focalMotorRange->setText(QString("%1-%2").arg(QString::number(m_profile.motorRange(x::MotorId_Focus).first)).arg(QString::number(m_profile.motorRange(x::MotorId_Focus).second)));
    ui->label_colorMotorRange->setText(QString("%1-%2").arg(QString::number(m_profile.motorRange(x::MotorId_Color).first)).arg(QString::number(m_profile.motorRange(x::MotorId_Color).second)));
    ui->label_chinHozMotorRange->setText(QString("%1-%2").arg(QString::number(m_profile.motorRange(x::MotorId_Chin_Hoz).first)).arg(QString::number(m_profile.motorRange(x::MotorId_Chin_Hoz).second)));
    ui->label_chinVertMotorRange->setText(QString("%1-%2").arg(QString::number(m_profile.motorRange(x::MotorId_Chin_Vert).first)).arg(QString::number(m_profile.motorRange(x::MotorId_Chin_Vert).second)));
    ui->label_videoSize->setText(QString("%1x%2").arg(QString::number(m_profile.videoSize().width())).arg(QString::number(m_profile.videoSize().height())));

    if(m_profile.devType()==0x8800) ui->label_devType->setText("布点");
    else if(m_profile.devType()==0x0088) ui->label_devType->setText("投射");
    else ui->label_devType->setText("未知");
    ui->label_devVer->setText(QString::number(m_profile.devVersion(),16));
}

void MainWindow::updateConfig()
{
    showDevInfo("Config Got.");
    memcpy(m_config.dataPtr(),m_devCtl->config().dataPtr(),UsbDev::Config::dataLen());
    showDevInfo(QString("文件校验码:")+QString::number(m_config.crcVeryficationRef()));
    quint32 crc=calcCrc((quint8*)m_config.dataPtr()+4, m_config.dataLen()-4);
    showDevInfo(QString("计算校验码:")+QString::number(crc));
    if(crc==m_config.crcVeryficationRef())
    {
        showDevInfo(QString("校验码一致。"));
    }
    else
    {
        showDevInfo(QString("校验码不一致。"));
    }
    initConfigUI();
    refreshConfigUI();
}

void MainWindow::on_pushButton_cameraSwitch_clicked()
{
    if(!m_statusData.isEmpty())
        m_devCtl->setFrontVideo(!m_statusData.cameraStatus());
    else
        showDevInfo("empty status");
}

void MainWindow::on_pushButton_chinMoveUp_pressed()
{
    moveChin(ChinMoveDirection::Up);
}

void MainWindow::on_pushButton_chinMoveUp_released()
{
   moveChin(ChinMoveDirection::Stop);
}

void MainWindow::on_pushButton_chinMoveDown_pressed()
{
    moveChin(ChinMoveDirection::Down);
}

void MainWindow::on_pushButton_chinMoveDown_released()
{
    moveChin(ChinMoveDirection::Stop);
}

void MainWindow::on_pushButton_chinMoveLeft_pressed()
{
    moveChin(ChinMoveDirection::Left);
}

void MainWindow::on_pushButton_chinMoveLeft_released()
{
    moveChin(ChinMoveDirection::Stop);
}

void MainWindow::on_pushButton_chinMoveRight_pressed()
{
    moveChin(ChinMoveDirection::Right);
}

void MainWindow::on_pushButton_chinMoveRight_released()
{
    moveChin(ChinMoveDirection::Stop);
}

void MainWindow::on_pushButton_light1_clicked()
{
    int index=ui->comboBox_lightSelect_1->currentIndex();
    int id;
    switch (index)
    {
    case 0:id=index;break;
    case 1:case 2:case 3:case 4:case 5:id=index+2;
    }

    m_devCtl->setLamp(UsbDev::DevCtl::LampId(id),0,ui->spinBox_centerLightAndOtherDA->value());

}

void MainWindow::on_pushButton_light2_clicked()
{
    UsbDev::DevCtl::LampId lampId=(UsbDev::DevCtl::LampId)(ui->comboBox_lightSelect_2->currentIndex()+1);
    m_devCtl->setLamp(lampId,ui->comboBox_lampIndex->currentIndex(),ui->spinBox_bigAndSmallDiamondDA->value());
}

void MainWindow::on_pushButton_light3_clicked()
{
    int r=ui->spinBox_lightR->value();
    int g=ui->spinBox_lightG->value();
    int b=ui->spinBox_lightB->value();
    m_devCtl->setWhiteLamp(r,g,b);
}

//void MainWindow::on_comboBox_lightSelect_currentIndexChanged(int index)
//{
//    ui->comboBox_lampNumber->setEnabled(index==1||index==2);
//    index==7?ui->label_da_RGB->setText("RGB:"):ui->label_da_RGB->setText("DA:");
//    switch(index)
//    {
//    case 7:ui->label_da_RGB->setText("RGB:");break;
//    case 8:ui->label_da_RGB->setText("PWM:");break;
//    default:ui->label_da_RGB->setText("DA:");break;
//    }
//    ui->spinBox_lightDAG->setEnabled(index==7);
//    ui->spinBox_lightDAB->setEnabled(index==7);
//}

void MainWindow::on_pushButton_testStart_clicked()
{
    if(m_devCtl==NULL) return;
    using MotorId=UsbDev::DevCtl::MotorId;
    quint8 sps[5];
    int spotSlot=ui->spinBox_spotSlot->value();
    int colorSlot=ui->spinBox_colorSlot->value();
    sps[0]=ui->spinBox_XMotorSpeed_2->value();
    sps[1]=ui->spinBox_YMotorSpeed_2->value();
    sps[2]=ui->spinBox_focalMotorSpeed_2->value();
    sps[3]=ui->spinBox_colorMotorSpeed_2->value();
    sps[4]=ui->spinBox_spotMotorSpeed_2->value();
    switch (ui->comboBox_testFucntion->currentIndex())
    {
        case 0:
        {
            if(m_status.currentColorSlot!=colorSlot||m_status.currentLightSpotPos!=spotSlot)              //变换到改变光斑颜色位置
            {
                int  color_Circl_Motor_Steps=m_profile.motorRange(UsbDev::DevCtl::MotorId_Color).second-m_profile.motorRange(UsbDev::DevCtl::MotorId_Color).first;
                int  spot_Circl_Motor_Steps=m_profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).second-m_profile.motorRange(UsbDev::DevCtl::MotorId_Light_Spot).first;

                {
                    int focalPos=m_config.focusPosForSpotAndColorChangeRef();
                    int motorPos[5]{0,0,focalPos,0,0};
                    waitMotorStop({UsbDev::DevCtl::MotorId_Focus});
                    m_devCtl->move5Motors(std::array<quint8, 5>{0,0,sps[2],0,0}.data(),motorPos);
//                    waitMotorStop({UsbDev::DevCtl::MotorId_Focus});
//                    m_devCtl->move5Motors(std::array<quint8, 5>{1,1,1,1,1}.data(),std::array<int, 5>{0,0,0,0,0}.data(),UsbDev::DevCtl::MoveMethod::Relative);//焦距到位立刻停止
                }
                showDevInfo("调整颜色和光斑.");
                waitForSomeTime(300);
                {
                    int motorPos[5]{0,0,0,ui->spinBox_colorMotorPos_2->value(),ui->spinBox_spotMotorPos_2->value()}; //单个电机绝对位置不可行
                    quint8 speed[5]{0,0,0,sps[3],sps[4]};
                    waitMotorStop({UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Focus});
                    m_devCtl->move5Motors(speed,motorPos);
                }
                waitForSomeTime(300);
                {
                    int  motorPos[5]{0,0,0,color_Circl_Motor_Steps,spot_Circl_Motor_Steps};
                    quint8 speed[5]{0,0,0,sps[3],sps[4]};
                    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});          //转一圈拖动光斑和颜色
                    m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);
                }
                waitForSomeTime(300);
                {
                    int motorPos[5]={0,0,0,-1000,-1000};
                    quint8 speed[5]{0,0,0,sps[3],sps[4]};
                    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot});
                    m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);    //防止干扰误差
                }
                waitForSomeTime(300);
                {
                    int motorPos[5]={0, 0, 10000, 0 ,0};
                    quint8 speed[5]{0,0,sps[2],0,0};
                    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus});
                    m_devCtl->move5Motors(speed,motorPos,UsbDev::DevCtl::Relative);     //DB 脱离颜色和光斑
                }
                m_status.currentColorSlot=colorSlot;
                m_status.currentColorSlot=spotSlot;
            }
            else{showDevInfo("颜色和光斑已经是目标位置.");}
            break;
        }
        case 1:
        {
            quint8 db=ui->spinBox_DbSetting->value();
            quint16 durationTime=ui->spinBox_shutterOpenDuration->text().toInt();
            quint32 shutterPos=ui->spinBox_shutterOpenPos->text().toInt();
            float coordX=ui->spinBox_coordX->text().toFloat();
            float coordY=ui->spinBox_coordY->text().toFloat();
            CoordSpacePosInfo coordSpacePosInfo{coordX,coordY};
            CoordMotorPosFocalDistInfo coordMotorPosFocalDistInfo;
            if(!getXYMotorPosAndFocalDistFromCoord(coordSpacePosInfo,coordMotorPosFocalDistInfo)) return;
            int focalMotorPos=ui->spinBox_focalMotorPos_2->text().toInt();
            staticCastTest(coordMotorPosFocalDistInfo,focalMotorPos,db,sps,durationTime,shutterPos);
            break;
        }
        case 2:
        {
            CoordSpacePosInfo dotBegin,dotEnd;
            dotBegin.coordX=ui->spinBox_beginCoordX->text().toInt();
            dotBegin.coordY=ui->spinBox_beginCoordY->text().toInt();
            dotEnd.coordX=ui->spinBox_endCoordX->text().toInt();
            dotEnd.coordY=ui->spinBox_endCoordY->text().toInt();

//            quint8 db=ui->spinBox_DbSetting->value();
            int spotSlot=ui->spinBox_spotSlot->value()-1;
            int speedLevel=ui->spinBox_speedLevel->value()-1;
            dynamicCastTest(dotBegin,dotEnd,spotSlot,speedLevel);
            if(waitForAnswer()) {showDevInfo("dynamic answered.");}
            break;
        }
        case 3:
        {
            m_isRunningDot=true;
            m_runDotCount=0;
            m_runDotLocs.clear();
            QString info=ui->plainTextEdit_runDotLocs->toPlainText().trimmed();
            QList<QString> infos=info.split(';');
            for(auto&i:infos)
            {
                auto loc=i.split(',');
                m_runDotLocs.append({loc[0].toInt(),loc[1].toInt()});
            }
            if(m_runDotLocs.length()==0) return;
            qsrand(QDateTime::currentSecsSinceEpoch());
            qDebug()<<m_runDotLocs;
            while(m_isRunningDot)
            {
                QPair<int,int> loc;
                if(m_runDotCount<m_runDotLocs.length())
                {
                    loc=m_runDotLocs[m_runDotCount];
                    m_runDotCount++;
                }
                else
                    loc=m_runDotLocs[qrand()%m_runDotLocs.length()];
                qDebug()<<loc;
                quint8 db=ui->spinBox_DbSetting->value();
                quint16 durationTime=ui->spinBox_shutterOpenDuration->text().toInt();
                quint32 shutterPos=ui->spinBox_shutterOpenPos->text().toInt();
                float coordX=loc.first;
                float coordY=loc.second;
                CoordSpacePosInfo coordSpacePosInfo{coordX,coordY};
                CoordMotorPosFocalDistInfo coordMotorPosFocalDistInfo;
                if(!getXYMotorPosAndFocalDistFromCoord(coordSpacePosInfo,coordMotorPosFocalDistInfo)) return;
                int focalMotorPos=getFocusMotorPosByDist(coordMotorPosFocalDistInfo.focalDist,spotSlot)+ui->lineEdit_focalMotorPosCorrection->text().toInt();
                staticCastTest(coordMotorPosFocalDistInfo,focalMotorPos,db,sps,durationTime,shutterPos);
                waitForSomeTime(200);
            }
            qDebug()<<"run dot over";
        }
    }
}



void MainWindow::on_comboBox_color_currentIndexChanged(int)
{
    QString text=ui->comboBox_color->currentText();
    for(auto &v:m_settings.m_colorToSlot)
    {
        if(v.first==text)
        {
            ui->spinBox_colorSlot->setValue(v.second);
        }
    }
}

void MainWindow::on_spinBox_colorSlot_valueChanged(int arg1)
{
    bool findColor=false;
    for(auto &v:m_settings.m_colorToSlot)
    {
         if(v.second==arg1)
         {
             ui->comboBox_color->setCurrentText(v.first);
             findColor=true;break;
         }
    }

    if(!findColor) ui->comboBox_color->setCurrentText("--");
    SET_BLOCKING_VALUE(ui->spinBox_colorMotorPos_2,m_config.switchColorMotorPosPtr()[arg1-1]);
}

void MainWindow::on_spinBox_colorMotorPos_2_valueChanged(int arg1)
{
    if(ui->checkBox_colorConfigSync->isChecked())
    {
        int colorSlot=ui->spinBox_colorSlot->value();
        m_config.switchColorMotorPosPtr()[colorSlot-1]=arg1;
    }
}

void MainWindow::on_comboBox_spotSize_currentIndexChanged(int)
{
   QString text=ui->comboBox_spotSize->currentText();
   for(auto &v:m_settings.m_spotSizeToSlot)
   {
       if(v.first==text) ui->spinBox_spotSlot->setValue(v.second);
   }
}

void MainWindow::on_spinBox_spotSlot_valueChanged(int arg1)
{
    bool findSpot=false;
    for(auto &v:m_settings.m_spotSizeToSlot)
    {
        if(v.second==arg1)
        {
            ui->comboBox_spotSize->setCurrentText(v.first);
            findSpot=true;break;
        }
    }
    if(!findSpot) ui->comboBox_spotSize->setCurrentText("--");
    SET_BLOCKING_VALUE(ui->spinBox_spotMotorPos_2,m_config.switchLightSpotMotorPosPtr()[arg1-1]);
}

void MainWindow::on_spinBox_spotMotorPos_2_valueChanged(int arg1)
{
    if(ui->checkBox_spotConfigSync->isChecked())
    {
        int spotSlot=ui->spinBox_spotSlot->value();
        m_config.switchLightSpotMotorPosPtr()[spotSlot-1]=arg1;
    }
}

void MainWindow::on_spinBox_DbSetting_valueChanged(int arg1)
{
   ui->spinBox_DbColorPos->setValue(m_config.DbPosMappingPtr()[arg1][0]);
   ui->spinBox_DbSpotPos->setValue(m_config.DbPosMappingPtr()[arg1][1]);
}

void MainWindow::on_spinBox_DbColorPos_valueChanged(int arg1)
{
    if(ui->checkBox_DbConfigSync->isChecked())
    {
        int db=ui->spinBox_DbSetting->value();
        m_config.DbPosMappingPtr()[db][0]=arg1;
    }
}

void MainWindow::on_spinBox_DbSpotPos_valueChanged(int arg1)
{
    if(ui->checkBox_DbConfigSync->isChecked())
    {
        int db=ui->spinBox_DbSetting->value();
        m_config.DbPosMappingPtr()[db][1]=arg1;
    }
}

void MainWindow::on_spinBox_focalMotorPos_2_valueChanged(int arg1)
{
    if(ui->checkBox_spotFocalSync->isChecked())
    {
        int slotIndex=ui->spinBox_spotSlot->value()-1;
        int focalDist=ui->spinBox_focalDist->value();
        int focalIndex=qRound(float(focalDist-80)/10);
//        m_localTableData.m_focalLengthMotorPosMappingData(focalIndex,slotIndex)=arg1;
        m_localTableData.m_focalLengthMotorPosMappingData(focalIndex,slotIndex)=arg1;
    }
}


void MainWindow::on_spinBox_shutterOpenPos_valueChanged(int arg1)
{
    if(ui->checkBox_shutterConfigSync->isChecked())
    {
        ui->lineEdit_shutterOpen->setText(QString::number(arg1));
    }
}

void MainWindow::on_pushButton_colorTest_clicked()
{
    quint8 sps[5]{0};
    int motorPos[5]{0};
    sps[3]=ui->spinBox_colorMotorSpeed_2->value();
    motorPos[3]=ui->spinBox_colorMotorPos_2->value();
//    m_devCtl->resetMotor(UsbDev::DevCtl::MotorId_Color,sps[3]);
//    waitMotorStop({UsbDev::DevCtl::MotorId_Color});
    //    QElapsedTimer mstimer;
    //    do
    //    {
    //        mstimer.restart();
    //        m_devCtl->move5Motors(sps,motorPos);
    //        while(m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId::MotorId_Color)||(mstimer.nsecsElapsed()/1000000<500))
    //        {
    //            QCoreApplication::processEvents();
    //        }
    //    }while(qAbs(m_statusData.motorPosition(UsbDev::DevCtl::MotorId::MotorId_Color)-motorPos[3])>500);//防止没移动到
     m_devCtl->move5Motors(sps,motorPos);
}

void MainWindow::on_pushButton_spotTest_clicked()
{
    quint8 sps[5]{0};
    int motorPos[5]{0};
    sps[4]=ui->spinBox_spotMotorSpeed_2->value();
    motorPos[4]=ui->spinBox_spotMotorPos_2->value();
//    m_devCtl->resetMotor(UsbDev::DevCtl::MotorId_Light_Spot,sps[4]);
//    waitMotorStop({UsbDev::DevCtl::MotorId_Light_Spot});
    m_devCtl->move5Motors(sps,motorPos);
}


void MainWindow::on_pushButton_dbTest_clicked()
{
    quint8 sps[5]{0};
    int motorPos[5]{0};
    sps[3]=ui->spinBox_focalMotorSpeed_2->value();
    motorPos[3]=ui->spinBox_DbColorPos->value();
    sps[4]=ui->spinBox_spotMotorSpeed_2->value();
    motorPos[4]=ui->spinBox_DbSpotPos->value();
    m_devCtl->move5Motors(sps,motorPos);
}

void MainWindow::on_pushButton_staticCastTest_clicked()
{
    quint8 sps[5]{0};
    int motorPos[5]{0};
    sps[0]=ui->spinBox_XMotorSpeed_2->value();
    sps[1]=ui->spinBox_YMotorSpeed_2->value();
    motorPos[0]=ui->spinBox_XMotorPos_2->value();
    motorPos[1]=ui->spinBox_YMotorPos_2->value();
    m_devCtl->move5Motors(sps,motorPos);
}

void MainWindow::on_pushButton_focalTest_clicked()
{
    quint8 sps[5]{0};
    int motorPos[5]{0};
    sps[2]=ui->spinBox_focalMotorSpeed_2->value();
    motorPos[2]=ui->spinBox_focalMotorPos_2->value();
    m_devCtl->move5Motors(sps,motorPos);
}

void MainWindow::on_pushButton_shuterTest_clicked()
{
    quint16 time = ui->spinBox_shutterOpenDuration->value();
    quint32 pos = ui->spinBox_shutterOpenPos->value();
    m_devCtl->openShutter(time,pos);
}


void MainWindow::on_comboBox_testFucntion_currentIndexChanged(int index)
{
    if(index==0) return;
    else
    {
        ui->stackedWidget->setCurrentIndex(index-1);
        ui->groupBox_focalTest->setEnabled(index==1);
    }
//    if(index==2)
//    {
//        ui->spinBox_spotSlot->setMinimum(1);
//    }
}




void MainWindow::on_tabWidget_currentChanged(int index)
{
//    if(index==2||index==3)
//    {
//        int width = GetSystemMetrics(SM_CXFULLSCREEN);
//        int height = GetSystemMetrics(SM_CYFULLSCREEN);
//        setMinimumSize(0, 0);
//        setMaximumSize(width,height);
//    }
//    else{
//        showNormal();
//        setFixedSize(m_width,m_height);
//    }
}

void MainWindow::on_action_readLocalData_triggered()
{

    QString filePath=QFileDialog::getOpenFileName(this,"打开文件",QDir::currentPath()+R"(/data/)",tr("(*.dat)"));
    readLocalData(filePath);
}

void MainWindow::readLocalData(QString filePath)
{
    QFile file(filePath);
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            char* destPtr=(char*)m_localTableData.m_data.data();
            int dataLen=m_localTableData.dataLen;
            QByteArray data=file.readAll();
//            if(data.length()!=dataLen)
//            {
//                showDevInfo(QString("文件长度错误:")+QString::number(data.length()));
//                showDevInfo(QString("文件长度应为:")+QString::number(dataLen));
//                return;
//            }
            auto len=qMin(dataLen,data.length());
            memcpy(destPtr,data.data(),len);
        }
        file.flush();
        file.close();
    }
}

void MainWindow::on_action_saveLocalData_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this,"打开文件",QDir::currentPath()+R"(/data/)",tr("(*.dat)"));
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly))
    {
        char* destPtr=(char*)m_localTableData.m_data.data();
        int dataLen=m_localTableData.dataLen;
//        QByteArray arr(dateLen,0xff);
//        file.write(arr);
        file.write(destPtr,dataLen);
        file.flush();
        file.close();
    }
}


void MainWindow::on_action_saveConfig_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this,"打开文件",QDir::currentPath()+R"(/data/)",tr("(*.cfg)"));
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly))
    {
        refreshConfigDataByUI();
        file.write((char*)m_config.dataPtr(),m_config.dataLen());
    }
}


void MainWindow::on_action_readConfigFromLocal_triggered()
{
    QString filePath=QFileDialog::getOpenFileName(this,"打开文件",QDir::currentPath()+R"(/data/)",tr("(*.cfg)"));
    readLocalConfig(filePath);
}

void MainWindow::readLocalConfig(QString filePath)
{
    QFile file(filePath);
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            QByteArray data=file.readAll();
            if(data.length()!=m_config.dataLen())
            {
                showDevInfo(QString("文件长度错误:")+QString::number(data.length()));
                showDevInfo(QString("文件长度应为:")+QString::number(m_config.dataLen()));
                return;
            }
            if(m_config.isEmpty())
            {
                m_config=UsbDev::Config(data);
            }
            else
            {
                m_config.setData(data);
            }


        }
        file.flush();
        file.close();
        showDevInfo(QString("文件校验码:")+QString::number(m_config.crcVeryficationRef()));
        quint32 crc=calcCrc((quint8*)m_config.dataPtr()+4, m_config.dataLen()-4);
        showDevInfo(QString("计算校验码:")+QString::number(crc));
        if(crc==m_config.crcVeryficationRef())
        {
            showDevInfo(QString("校验码一致。"));
        }
        else
        {
            showDevInfo(QString("校验码不一致。"));
        }
        initConfigUI();
        refreshConfigUI();
    }
}

quint16 MainWindow::calcCrc(quint8 *p_data, int data_len)
{
    int32_t i;
    uint16_t crc_value = 0xFFFF; /* 定义一个16位无符号类型的变量，并初始化为0xFFFF */

    while(data_len--)
    {
        /* 数据包中的字节与CRC变量中的低字节进行异或运算，结果存回CRC变量 */
        crc_value ^= *p_data++;
        for (i = 0; i < 8; i++) {
            if (crc_value & 0x0001) {
                /* 如果最低位为1：将CRC变量与固定值0xA001进行异或运算 */
                crc_value = (crc_value >> 1) ^ 0xA001;
            } else {
                /* 如果最低位为0：重复第3步(配合计算流程来阅读代码) */
                crc_value >>= 1;
            }
        }
    }
    return crc_value;
}

void MainWindow::on_action_updateConfigToLower_triggered()
{
    qDebug()<<m_config.centerFixationLampDARef();
    refreshConfigDataByUI();
    qDebug()<<m_config.centerFixationLampDARef();
    m_devCtl->saveConfig(m_config);
}

void MainWindow::on_action_downloadConfig_triggered()
{
    m_devCtl->readConfig();

}

void MainWindow::on_spinBox_centerLightAndOtherDA_valueChanged(int arg1)
{
    if(ui->checkBox_centerLightAndOtherDASync->isChecked())
    {
        int index=ui->comboBox_lightSelect_1->currentIndex();
        switch (index)
        {
        case 0:ui->lineEdit_centralLightDA->setText(QString::number(arg1));break;
        case 1:ui->lineEdit_yellowBackGroundLampDa->setText(QString::number(arg1));break;
        case 2:ui->lineEdit_centerInfraredLampDA->setText(QString::number(arg1));break;
        case 3:ui->lineEdit_borderLampDA->setText(QString::number(arg1));break;
        case 4:ui->lineEdit_eyeGlassLampDa->setText(QString::number(arg1));break;
        }
    }
}

void MainWindow::on_spinBox_bigAndSmallDiamondDA_valueChanged(int arg1)
{
    if(ui->checkBox_spinBox_bigAndSmallDiamondDASync->isChecked())
    {
        int index=ui->comboBox_lightSelect_2->currentIndex();
        int index2=ui->comboBox_lampIndex->currentIndex();
        switch (index)
        {
        case 0:m_config.bigDiamondfixationLampDAPtr()[index2]=arg1;break;
        case 1:m_config.smallDiamondFixationLampDAPtr()[index2]=arg1;break;
        }
        refreshConfigUI();
    }
}

void MainWindow::on_spinBox_lightR_valueChanged(int arg1)
{
    if(ui->checkBox_whiteLightDASync->isChecked())
        m_config.whiteBackgroundLampDAPtr()[0]=arg1;
    refreshConfigUI();
}

void MainWindow::on_spinBox_lightG_valueChanged(int arg1)
{
    if(ui->checkBox_whiteLightDASync->isChecked())
        m_config.whiteBackgroundLampDAPtr()[1]=arg1;
    refreshConfigUI();
}

void MainWindow::on_spinBox_lightB_valueChanged(int arg1)
{
    if(ui->checkBox_whiteLightDASync->isChecked())
        m_config.whiteBackgroundLampDAPtr()[2]=arg1;
    refreshConfigUI();
}

void MainWindow::on_comboBox_lightSelect_1_currentIndexChanged(int index)
{

//    int index=ui->comboBox_lightSelect_1->currentIndex();
    int value;
    switch (index)
    {
    case 0:ui->checkBox_centerLightAndOtherDASync->setEnabled(true);value=m_config.centerFixationLampDARef();ui->spinBox_centerLightAndOtherDA->setValue(value);break;
    case 1:ui->checkBox_centerLightAndOtherDASync->setEnabled(true);value=m_config.yellowBackgroundLampDARef();ui->spinBox_centerLightAndOtherDA->setValue(value);break;
    case 2:ui->checkBox_centerLightAndOtherDASync->setEnabled(true);value=m_config.centerInfraredLampDARef();ui->spinBox_centerLightAndOtherDA->setValue(value);break;
    case 3:ui->checkBox_centerLightAndOtherDASync->setEnabled(true);value=m_config.borderInfraredLampDARef();ui->spinBox_centerLightAndOtherDA->setValue(value);break;
    case 4:ui->checkBox_centerLightAndOtherDASync->setEnabled(true);value=m_config.eyeglassFrameLampDARef();ui->spinBox_centerLightAndOtherDA->setValue(value);break;
    case 5:ui->checkBox_centerLightAndOtherDASync->setEnabled(false);ui->checkBox_centerLightAndOtherDASync->setChecked(false);;break;
    }

//    refreshConfigUI();

}

void MainWindow::on_comboBox_lightSelect_2_currentIndexChanged(int index)
{
    ui->comboBox_lampIndex->setCurrentIndex(0);
    ui->comboBox_lampIndex->currentIndexChanged(0);
}

void MainWindow::on_comboBox_lampIndex_currentIndexChanged(int index)
{
    int bigOrsmall=ui->comboBox_lightSelect_2->currentIndex();
    if(bigOrsmall==0)
    {
        ui->spinBox_bigAndSmallDiamondDA->setValue(m_config.bigDiamondfixationLampDAPtr()[index]);
    }
    else
    {
        ui->spinBox_bigAndSmallDiamondDA->setValue(m_config.smallDiamondFixationLampDAPtr()[index]);
    }
}

void MainWindow::on_pushButton_readCache_clicked()
{

    if(ui->comboBox_testFucntion->currentIndex()==0)
    {
        UsbDev::StaticCache* staticCache= m_devCtl->readStaticCache();
        if(staticCache==Q_NULLPTR) return;
        for(int i=0;i<3;i++)
        {
            auto v=staticCache[i];
            showDevInfo(QString("缓存序号:%1.").arg(i));
            showDevInfo(QString("刺激点编号:%1.X电机位置:%2.Y电机位置:%3."
                                "快门打开时间:%4.应答器按下时间:%5.应答器松开时间:%6.\n")
                        .arg(v.stimulateDotSerialNumber).arg(v.motorPosX).arg(v.motorPosY).
                        arg(v.shutterTime).arg(v.answerPadPressedTime).arg(v.answerPadRealeasedTime));
        }

    }
    else
    {
        UsbDev::MoveCache* moveCache=m_devCtl->readMoveCache();
        if(moveCache==Q_NULLPTR) return;
        for(int i=0;i<3;i++)
        {
            auto v=moveCache[i];
            showDevInfo(QString("缓存序号:%1.").arg(i));
            showDevInfo(QString(R"(刺激点编号:%1.X电机位置:%2.Y电机位置:%3.)").arg(v.stimulateDotSerialNumber).arg(v.motorPosX).arg(v.motorPosY));
        }
    }
}

void MainWindow::on_checkBox_IO_stateChanged(int arg1)
{
    arg1==Qt::CheckState::Checked?connect(m_devCtl,&UsbDev::DevCtl::updateIOInfo,this,&MainWindow::showDevInfo):
                                  disconnect(m_devCtl,&UsbDev::DevCtl::updateIOInfo,this,&MainWindow::showDevInfo);
}

void MainWindow::on_checkBox_RefreshIO_stateChanged(int arg1)
{
    arg1==Qt::CheckState::Checked?connect(m_devCtl,&UsbDev::DevCtl::updateRefreshIOInfo,this,&MainWindow::showDevRefreshInfo):
                                  disconnect(m_devCtl,&UsbDev::DevCtl::updateRefreshIOInfo,this,&MainWindow::showDevRefreshInfo);

}

void MainWindow::on_rawComand_clicked()
{
    auto text=ui->plainTextEdit_rawCommand->toPlainText();
    text=text.trimmed();
    auto strings=text.split(" ");
    QByteArray ba;
    bool ok;
    for(auto&i:strings)
    {
        auto locAndValue=i.split(':');
        quint8 byte;
        if(locAndValue.length()==2)
            byte=locAndValue[1].toUInt(&ok,16);
        else
            byte=i.toUInt(&ok,16);
        ba.append(byte);
    }
    if(ok)
        m_devCtl->sendBinaryCommand(ba,strings.count());

}

void MainWindow::on_action_connectDev_triggered()
{
    if(m_devCtl!=NULL)
    {
        uninitDevCtl();
    }
    initDevCtl();
}

void MainWindow::on_action_disconnect_triggered()
{
    uninitDevCtl();
    m_devCtl=NULL;
}

void MainWindow::on_pushButton_stopDynamic_clicked()
{
    m_devCtl->stopDyanmic();
}

void MainWindow::on_pushButton_beep_clicked()
{
    quint16 repeatCount=ui->spinBox_BeepCount->value();
    quint16 beepDuration=ui->spinBox_beepDurationTime->value();
    quint16 beepInterval=ui->spinBox_beepIntervalTime->value();

    m_devCtl->beep(repeatCount,beepDuration,beepInterval);
}

void MainWindow::on_pushButton_stopRunDot_clicked()
{
    m_isRunningDot=false;
}

void MainWindow::on_pushButton_takePhoto_clicked()
{
    int inteval=ui->spinBox_takingPhotoInteval->text().toInt();
    if(inteval==0)
    {
        m_takePhoto=true;
    }
    else
    {
        m_keepTakingPhoto=!m_keepTakingPhoto;
        m_keepTakingPhoto?ui->pushButton_takePhoto->setText("停止"):ui->pushButton_takePhoto->setText("开始");
        if(m_keepTakingPhoto)
        {
            m_takePhoto=true;
            m_takingPhotoTimer->setInterval(inteval);
            m_takingPhotoTimer->start();
            ui->spinBox_takingPhotoInteval->setEnabled(false);
        }
        else
        {
            m_takingPhotoTimer->stop();
            ui->spinBox_takingPhotoInteval->setEnabled(true);
        }
    }
}

void MainWindow::on_plainTextEdit_rawCommand_textChanged()
{
    static int previousWordCount=0;
    auto text=ui->plainTextEdit_rawCommand->toPlainText();
    int wordCount=text.size();
    if(wordCount>previousWordCount)
    {
        if(text.size()%3==2)
        {
            text.append(" ");
            ui->plainTextEdit_rawCommand->setPlainText(text);
            ui->plainTextEdit_rawCommand->moveCursor(QTextCursor::EndOfBlock);
        }
    }
    previousWordCount=wordCount;
}

void MainWindow::on_checkBox_startRefreshInfo_stateChanged(int arg1)
{

    if(arg1==Qt::CheckState::Checked)
    {
        connect(m_devCtl,&UsbDev::DevCtl::updateRefreshInfo,this,&MainWindow::showDevRefreshInfo);
        connect(this,&MainWindow::updateRefreshInfo,this,&MainWindow::showDevRefreshInfo);
        ui->checkBox_RefreshIO->setEnabled(true);
    }
    else
    {
        disconnect(m_devCtl,&UsbDev::DevCtl::updateRefreshInfo,this,&MainWindow::showDevRefreshInfo);
        disconnect(this,&MainWindow::updateRefreshInfo,this,&MainWindow::showDevRefreshInfo);
        ui->checkBox_RefreshIO->setCheckState(Qt::CheckState::Unchecked);
        ui->checkBox_RefreshIO->setEnabled(false);
    }
}


void MainWindow::on_pushButton_relativeMoveChin_clicked()
{ moveChinMotors(UsbDev::DevCtl::MoveMethod::Relative); }

void MainWindow::on_pushButton_absoluteMoveChin_clicked()
{ moveChinMotors(UsbDev::DevCtl::MoveMethod::Abosolute); }


void MainWindow::showDevInfo(QString str)
{ ui->textBrowser_infoText->append(str);}

void MainWindow::showDevRefreshInfo(QString str)
{ ui->textBrowser_refreshInfoText->append(str);}

void MainWindow::on_pushButton_relativeMove5Motors_clicked()
{move5Motors(UsbDev::DevCtl::MoveMethod::Relative);}

void MainWindow::on_pushButton_absoluteMove5Motors_clicked()
{move5Motors(UsbDev::DevCtl::MoveMethod::Abosolute);}

void MainWindow::on_pushButton_resetCheckedMotors_clicked()
{
    if(m_devCtl==NULL) return;
    UsbDev::DevCtl::MotorId motorid;
    if(ui->radioButton_xMotor->isChecked()) motorid = UsbDev::DevCtl::MotorId::MotorId_X;
    if(ui->radioButton_yMotor->isChecked()) motorid = UsbDev::DevCtl::MotorId::MotorId_Y;
    if(ui->radioButton_colorMotor->isChecked()) motorid = UsbDev::DevCtl::MotorId::MotorId_Color;
    if(ui->radioButton_focusMotor->isChecked()) motorid = UsbDev::DevCtl::MotorId::MotorId_Focus;
    if(ui->radioButton_spotMotor->isChecked()) motorid = UsbDev::DevCtl::MotorId::MotorId_Light_Spot;
    if(ui->radioButton_shutterMotor->isChecked()) motorid = UsbDev::DevCtl::MotorId::MotorId_Shutter;
    if(ui->radioButton_chinHozMotor->isChecked()) motorid = UsbDev::DevCtl::MotorId::MotorId_Chin_Hoz;
    if(ui->radioButton_chinVertMotor->isChecked()) motorid = UsbDev::DevCtl::MotorId::MotorId_Chin_Vert;
    m_devCtl->resetMotor(motorid,ui->spinBox_resetSpeed->value());
}

void MainWindow::moveChinMotors(UsbDev::DevCtl::MoveMethod method)
{
    if(m_devCtl==NULL) return;
    qint32 value[2]={0};
    quint8 speed[2]={0};
    if(ui->checkBox_testChinHoz->isChecked())
    {
        value[0] = ui->spinBox_hozChinMotorPos->value();
        speed[0] = ui->spinBox_hosChinMotorSpeed->value();
    }
    if(ui->checkBox_testChinVert->isChecked())
    {
        value[1] = ui->spinBox_vertChinMotorPos->value();
        speed[1] = ui->spinBox_vertChinMotorSpeed->value();
    }
    m_devCtl->moveChinMotors(speed,value,method);
}



void MainWindow::moveChin(ChinMoveDirection direction)
{

    auto profile=m_devCtl->profile();
    quint8 spsConfig[2]={ui->spinBox_speedChinMove->value(),ui->spinBox_speedChinMove->value()};
    quint8 sps[2]{0,0};
    int motorPos[2]{0,0};
    switch(direction)
    {
    case ChinMoveDirection::Left:
    {
        sps[0]=spsConfig[0];
        motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).second;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }
    case ChinMoveDirection::Right:
    {
        sps[0]=spsConfig[0];
        motorPos[0]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Hoz).first;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }
    case ChinMoveDirection::Up:
    {
        sps[1]=spsConfig[1];
        motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).second;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }
    case ChinMoveDirection::Down:
    {
        sps[1]=spsConfig[1];
        motorPos[1]=profile.motorRange(UsbDev::DevCtl::MotorId_Chin_Vert).first;
        m_devCtl->moveChinMotors(sps,motorPos);
        break;
    }

    case ChinMoveDirection::Stop:
    {
        m_devCtl->moveChinMotors(std::array<quint8,2>{1,1}.data(),std::array<qint32,2>{0,0}.data(),UsbDev::DevCtl::MoveMethod::Relative);
        break;
    }
    }
}


void MainWindow::move5Motors(UsbDev::DevCtl::MoveMethod method)
{
    if(m_devCtl==NULL) return;
    qint32 value[5]{0};
    quint8 speed[5]{0};
    if(ui->checkBox_testX->isChecked())
    {
        value[0]=ui->spinBox_XMotorPos->value();
        speed[0]=ui->spinBox_XMotorSpeed->value();
    }
    if(ui->checkBox_testY->isChecked())
    {
        value[1]=ui->spinBox_YMotorPos->value();
        speed[1]=ui->spinBox_YMotorSpeed->value();
    }
    if(ui->checkBox_testFocus->isChecked())
    {
        value[2]=ui->spinBox_focalMotorPos->value();
        speed[2]=ui->spinBox_focalMotorSpeed->value();
    }
    if(ui->checkBox_testColor->isChecked())
    {
        value[3]=ui->spinBox_colorMotorPos->value();
        speed[3]=ui->spinBox_colorMotorSpeed->value();
    }
    if(ui->checkBox_testSpot->isChecked())
    {
        value[4]=ui->spinBox_spotMotorPos->value();
        speed[4]=ui->spinBox_spotMotorSpeed->value();
    }
    m_devCtl->move5Motors(speed,value,method);
}

void MainWindow::fillXYMotorAndFocalInfoByXYCoord()
{
    if(!ui->checkBox_calcFocalDist->isChecked()) return;
    CoordSpacePosInfo coordSpacePosInfo;
    coordSpacePosInfo.coordX=ui->spinBox_coordX->value();
    coordSpacePosInfo.coordY=ui->spinBox_coordY->value();
    CoordMotorPosFocalDistInfo coordMotorPosFocalDistInfo;
    getXYMotorPosAndFocalDistFromCoord(coordSpacePosInfo,coordMotorPosFocalDistInfo);
    int focalMotorPos=getFocusMotorPosByDist(coordMotorPosFocalDistInfo.focalDist,ui->spinBox_spotSlot->value())+ui->lineEdit_focalMotorPosCorrection->text().toInt();
    ui->spinBox_XMotorPos_2->setValue(coordMotorPosFocalDistInfo.motorX);
    ui->spinBox_YMotorPos_2->setValue(coordMotorPosFocalDistInfo.motorY);
    ui->spinBox_focalDist->setValue(coordMotorPosFocalDistInfo.focalDist);
    SET_BLOCKING_VALUE(ui->spinBox_focalMotorPos_2,focalMotorPos);             //移动位置的时候,同步焦距到本地数据.
//    ui->spinBox_focalMotorPos_2->setValue(focalMotorPos);

}

bool MainWindow::getXYMotorPosAndFocalDistFromCoord(const CoordSpacePosInfo& coordSpacePosInfo,CoordMotorPosFocalDistInfo& coordMotorPosFocalDistInfo)
{
    static bool isMainDotInfoTable;
//    if(isMainDotInfoTable&&coordSpacePosInfo.coordX>30){isMainDotInfoTable=false;}
//    if(!isMainDotInfoTable&&coordSpacePosInfo.coordX<-30){isMainDotInfoTable=true;}
    if(ui->radioButton_mainTable->isChecked()) isMainDotInfoTable=true;
    else if(ui->radioButton_secondaryTable->isChecked()) isMainDotInfoTable=false;
    //有15格,所以要加15,Y要反号
    int x1=floor(coordSpacePosInfo.coordX/6.0f)+15;int x2=ceil(coordSpacePosInfo.coordX/6.0f)+15;
    int y1=floor(-coordSpacePosInfo.coordY/6.0f)+15;int y2=ceil(-coordSpacePosInfo.coordY/6.0f)+15;
//    qDebug()<<"x1:"<<x1<<"x2:"<<x2;
//    qDebug()<<"y1:"<<y1<<"y2:"<<y2;
    auto data=m_localTableData;
    SingleTableData tableData;
    isMainDotInfoTable?tableData=data.m_mainPosTableData:tableData=data.m_secondaryPosTableData;

    CoordMotorPosFocalDistInfo fourDots[4]              //周围四个坐标点的马达和焦距值
    {
        {tableData(y1*3,x1),tableData(y1*3+1,x1),tableData(y1*3+2,x1)},{tableData(y1*3,x2),tableData(y1*3+1,x2),tableData(y1*3+2,x2)},
        {tableData(y2*3,x1),tableData(y2*3+1,x1),tableData(y2*3+2,x1)},{tableData(y2*3,x2),tableData(y2*3+1,x2),tableData(y2*3+2,x2)},
    };
    if(((fourDots[0].motorX==-1)||(fourDots[1].motorX==-1)||(fourDots[2].motorX==-1)||(fourDots[3].motorX==-1)))
    {showDevInfo("point is out of range!");}
//    if(!((fourDots[0].motorX!=-1)&&(fourDots[1].motorX!=-1)&&(fourDots[2].motorX!=-1)&&(fourDots[3].motorX!=-1)))
//    {
//        isMainDotInfoTable=!isMainDotInfoTable;
//        isMainDotInfoTable?tableData=data.m_mainPosTableData:tableData=data.m_secondaryPosTableData;
//        fourDots[0]={tableData(y1*3,x1),tableData(y1*3+1,x1),tableData(y1*3+2,x1)};
//        fourDots[1]={tableData(y2*3,x1),tableData(y2*3+1,x1),tableData(y2*3+2,x1)};
//        fourDots[2]={tableData(y1*3,x2),tableData(y1*3+1,x2),tableData(y1*3+2,x2)};
//        fourDots[3]={tableData(y2*3,x2),tableData(y2*3+1,x2),tableData(y2*3+2,x2)};
//        if(!((fourDots[0].motorX=!-1)||(fourDots[1].motorX=-1)||(fourDots[2].motorX=-1)||(fourDots[3].motorX=-1)))
//        {
//            showDevInfo("point is out of range!");
//            isMainDotInfoTable=!isMainDotInfoTable;
//            return false;
//        }
//    }


    QPointF loc(coordSpacePosInfo.coordX-(x1-15)*6,-coordSpacePosInfo.coordY-(y1-15)*6);         //算出比格子位置多多少
    int arr[4];
    for(unsigned int i=0;i<sizeof(arr)/sizeof(int);i++) {arr[i]=fourDots[i].motorX;}            //四个格子差值计算
    coordMotorPosFocalDistInfo.motorX=interpolation(arr,loc);

    for(unsigned int i=0;i<sizeof(arr)/sizeof(int);i++) {arr[i]=fourDots[i].motorY;}
    coordMotorPosFocalDistInfo.motorY=interpolation(arr,loc);

    if(isMainDotInfoTable)
    {
        coordMotorPosFocalDistInfo.motorX+=ui->lineEdit_centerX->text().toInt();
        coordMotorPosFocalDistInfo.motorY+=ui->lineEdit_centerY->text().toInt();
    }
    else
    {
        coordMotorPosFocalDistInfo.motorX+=ui->lineEdit_secondaryCenterX->text().toInt();
        coordMotorPosFocalDistInfo.motorY+=ui->lineEdit_secondaryCenterY->text().toInt();
    }

    for(unsigned int i=0;i<sizeof(arr)/sizeof(int);i++) {arr[i]=fourDots[i].focalDist;}
    coordMotorPosFocalDistInfo.focalDist=interpolation(arr,loc);
//    showDevInfo(QString("X电机:%1,Y电机:%2,焦距:%3.").
//                arg(QString::number(coordMotorPosFocalDistInfo.motorX)).
//                arg(QString::number(coordMotorPosFocalDistInfo.motorY)).
//                arg(QString::number(coordMotorPosFocalDistInfo.focalDist)));
//    qDebug()<<QString("X电机:%1,Y电机:%2,焦距:%3.").
//              arg(QString::number(coordMotorPosFocalDistInfo.motorX)).
//              arg(QString::number(coordMotorPosFocalDistInfo.motorY)).
//              arg(QString::number(coordMotorPosFocalDistInfo.focalDist));
    return true;
}

void MainWindow::staticCastTest( CoordMotorPosFocalDistInfo& coordMotorPosFocalDistInfo,int focalMotorPos,int db,quint8* sps,int durationTime,int shutterPos)
{
    if(m_devCtl==NULL) return;
//    以后加上
//    if(m_config.isEmpty()) {showDevInfo("empty config"); return;}
//    while(m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_X)||m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_Y)||
//          m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_Focus)||m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_Color)||
//          m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_Light_Spot)||m_statusData.isMotorBusy(UsbDev::DevCtl::MotorId_Shutter))
//    {QCoreApplication::processEvents();}

    //调整DB同时移动到指定位置
    showDevInfo("调整焦距DB同时移动到指定位置.");
    {
        int motorPos[5]{0};
        motorPos[0]=coordMotorPosFocalDistInfo.motorX;
        motorPos[1]=coordMotorPosFocalDistInfo.motorY;
        motorPos[2]=focalMotorPos;
        motorPos[3]=m_config.DbPosMappingPtr()[db][0];
        motorPos[4]=m_config.DbPosMappingPtr()[db][1];


//        motorPos[2]=getFocusMotorPosByDist(coordMotorPosFocalDistInfo.focalDist,spotSlot);
        waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_X,UsbDev::DevCtl::MotorId_Y});
        m_devCtl->move5Motors(sps,motorPos);
    }

    //打开快门
    showDevInfo("打开快门.");
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,UsbDev::DevCtl::MotorId_Light_Spot,UsbDev::DevCtl::MotorId_Focus,UsbDev::DevCtl::MotorId_X,UsbDev::DevCtl::MotorId_Y,UsbDev::DevCtl::MotorId_Shutter});
    m_devCtl->openShutter(durationTime,shutterPos);
}


void MainWindow::dynamicCastTest(CoordSpacePosInfo& dotSpaceBegin,CoordSpacePosInfo& dotSpaceEnd,int spotSlot,int speedLevel)
{
//    dotSpaceBegin.coordX=3.66054f;
//    dotSpaceBegin.coordY=41.8402f;
//    dotSpaceEnd.coordX=0;
//    dotSpaceEnd.coordY=0;
//    qDebug()<<dotSpaceBegin.coordX;
//    qDebug()<<dotSpaceBegin.coordY;
//    qDebug()<<dotSpaceEnd.coordX;
//    qDebug()<<dotSpaceEnd.coordY;
    auto data=m_localTableData.m_dynamicLenAndTimeData;
    auto stepLength=float(data(speedLevel,0))*0.01;
    auto stepTime=data(speedLevel,1);
    float stepLengthX,stepLengthY;
    float distX=dotSpaceEnd.coordX-dotSpaceBegin.coordX;
    float distY=dotSpaceEnd.coordY-dotSpaceBegin.coordY;
    CoordSpacePosInfo coordSpacePosInfoTemp=dotSpaceBegin;
    CoordMotorPosFocalDistInfo coordMotorPosFocalDistInfoTemp;
    getXYMotorPosAndFocalDistFromCoord(dotSpaceBegin,coordMotorPosFocalDistInfoTemp);
    auto focalMotorPos=getFocusMotorPosByDist(coordMotorPosFocalDistInfoTemp.focalDist,spotSlot)+ui->lineEdit_focalMotorPosCorrection->text().toInt();
    int motorPos[5];
    motorPos[0]=coordMotorPosFocalDistInfoTemp.motorX;
    motorPos[1]=coordMotorPosFocalDistInfoTemp.motorY;
    motorPos[2]=focalMotorPos;
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,
                   UsbDev::DevCtl::MotorId_Light_Spot,
                   UsbDev::DevCtl::MotorId_Focus,
                   UsbDev::DevCtl::MotorId_X,
                   UsbDev::DevCtl::MotorId_Y
                   });
    quint8 sps[5];
    sps[0]=ui->spinBox_XMotorSpeed_2->value();
    sps[1]=ui->spinBox_YMotorSpeed_2->value();
    sps[2]=ui->spinBox_focalMotorSpeed_2->value();
    sps[3]=0;
    sps[4]=0;
    m_devCtl->move5Motors(sps,motorPos);
    int stepCount;
    if(std::abs(distX)>std::abs(distY))
    {
        distX>0?stepLengthX=stepLength:stepLengthX=-stepLength;
        stepCount=qCeil(distX/stepLengthX);
    }
    else
    {
        distY>0?stepLengthY=stepLength:stepLengthY=-stepLength;
        stepCount=qCeil(distY/stepLengthY);
    }

    stepLengthX=distX/stepCount;
    stepLengthY=distY/stepCount;

    int* dotArr=new int[stepCount*3];

    showDevInfo(QString("分割为%1步,X步长为%2,Y步长为%3.").arg(QString::number(stepCount)).arg(QString::number(stepLengthX)).arg(QString::number(stepLengthY)));

    for(int i=0;i<stepCount;i++)
    {
        coordSpacePosInfoTemp.coordX+=stepLengthX;
        coordSpacePosInfoTemp.coordY+=stepLengthY;
        getXYMotorPosAndFocalDistFromCoord(coordSpacePosInfoTemp,coordMotorPosFocalDistInfoTemp);
        dotArr[i*3+0]=coordMotorPosFocalDistInfoTemp.motorX;
        dotArr[i*3+1]=coordMotorPosFocalDistInfoTemp.motorY;
        dotArr[i*3+2]=getFocusMotorPosByDist(coordMotorPosFocalDistInfoTemp.focalDist,spotSlot)+ui->lineEdit_focalMotorPosCorrection->text().toInt();
        showDevInfo((QString("第%1个点,X坐标:%2,Y坐标:%3,X电机坐标%4,Y电机坐标%5,焦距电机坐标%6.")
                   .arg(QString::number(i)).arg(QString::number(coordSpacePosInfoTemp.coordX)).arg(QString::number(coordSpacePosInfoTemp.coordY)).arg(QString::number( dotArr[i*3+0])).
                    arg(QString::number( dotArr[i*3+1])).arg(QString::number( dotArr[i*3+2]))));
    }

    showDevInfo(("发送移动数据"));
    constexpr int maxPackageLen=512;
    constexpr int stepPerFrame=(maxPackageLen-8)/(4*3);
    int totalframe=ceil((float)stepCount/stepPerFrame);
    showDevInfo(QString("分割为%1帧").arg(QString::number(totalframe)));
    for(int i=0;i<totalframe-1;i++)
        m_devCtl->sendDynamicData(totalframe,i,512,&dotArr[stepPerFrame*3*i]);                        //一般帧

//    qDebug()<<dotArr[(stepCount-1)*3];
//    qDebug()<<dotArr[(stepCount-1)*3+1];
//    qDebug()<<dotArr[(stepCount-1)*3+2];

    int remainStep=stepCount-stepPerFrame*(totalframe-1);
    int dataLen= remainStep*3*4+8;
    qDebug()<<dataLen;
    m_devCtl->sendDynamicData(totalframe,totalframe-1,dataLen,&dotArr[stepPerFrame*3*(totalframe-1)]);     //最后一帧

//    m_devCtl->sendDynamicData(1,0,512,&dotArr[0]);     //最后一帧
    showDevInfo(("开始移动"));
    waitMotorStop({UsbDev::DevCtl::MotorId_Color,
                   UsbDev::DevCtl::MotorId_Light_Spot,
                   UsbDev::DevCtl::MotorId_Focus,
                   UsbDev::DevCtl::MotorId_X,
                   UsbDev::DevCtl::MotorId_Y
                   });
    m_devCtl->openShutter(65535,ui->lineEdit_shutterOpen->text().toInt());
    m_devCtl->startDynamic(speedLevel,speedLevel,speedLevel,stepTime,stepCount);    //开始

    delete[] dotArr;
}

bool MainWindow::waitForAnswer()
{
    while(!m_statusData.moveStatus())
        QApplication::processEvents();          //等待刷新状态
    while(m_statusData.moveStatus())
    {
        if(m_statusData.answerpadStatus())
        {
            m_devCtl->stopDyanmic();
            return true;
        }
        QApplication::processEvents();
    }
    return false;
}





