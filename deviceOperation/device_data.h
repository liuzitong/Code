#ifndef DEVICE_DATA_H
#define DEVICE_DATA_H
#include <QObject>
#include <QSharedPointer>
#include <usbdev/main/usbdev_config.hxx>

namespace DevOps{

struct CoordMotorPosFocalDistInfo
{
    int motorX,motorY,focalDist;
//    CoordMotorPosFocalDistInfo()=default;
//    CoordMotorPosFocalDistInfo(int x,int y,int f);
//    CoordMotorPosFocalDistInfo operator=(CoordMotorPosFocalDistInfo other);
//    CoordMotorPosFocalDistInfo& operator=(CoordMotorPosFocalDistInfo& other);
//    CoordMotorPosFocalDistInfo& operator=(CoordMotorPosFocalDistInfo&& other);
};


struct SingleTableData
{
    SingleTableData()=default;
    SingleTableData(int row,int column,int* data):m_row(row),m_column(column),m_data(data){};
    ~SingleTableData()=default;

    int& operator()(int row,int column){return m_data[row*m_column+column];}

    int m_row;
    int m_column;
    int* m_data;
};

class LocalTableData
{
public:
    LocalTableData();
    ~LocalTableData(){};

public:
    SingleTableData m_mainPosTableData;       //正副表
    SingleTableData m_secondaryPosTableData;
    SingleTableData m_dbAngleDampingTableData;  //DB角度衰减表
    SingleTableData m_xyDistTableData;          //焦距参数表
    SingleTableData m_focalLengthMotorPosMappingData;    //光斑在不同焦距对应的电机焦距步  25种焦距 6种光斑
    SingleTableData m_dynamicLenAndTimeData;    //不同速度对应步长和时间  一共9对
    QSharedPointer<int> m_data;
    const int dataLen=(31*31*3*2+46+25*2+25*6+9*2)*sizeof(int);     //正副表，DB角度衰减表，焦距参数表,投射表
};


class DeviceData:public QObject
{
    Q_OBJECT

public:
    DeviceData();
    static QSharedPointer<DeviceData> getSingleton();
    LocalTableData m_localTableData;
    UsbDev::Config m_config;


private:
    static QSharedPointer<DeviceData> m_singleton;
    bool readLocalConfig(QString filePath);
    bool readLocalData(QString filePath);

};
}

#endif // DEVICE_DATA_H
