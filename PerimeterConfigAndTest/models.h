#ifndef MODELS_H
#define MODELS_H
#include <QSharedPointer>

struct Status
{
   int currentColorSlot;
   int currentLightSpotPos;
   int currentDB;
};

struct CoordSpacePosInfo
{
    float coordX,coordY;
};

struct CoordMotorPosFocalDistInfo
{
    int motorX,motorY,focalDist;
    CoordMotorPosFocalDistInfo()=default;
    CoordMotorPosFocalDistInfo(int x,int y,int f);
    CoordMotorPosFocalDistInfo& operator=(CoordMotorPosFocalDistInfo& other);
    CoordMotorPosFocalDistInfo& operator=(CoordMotorPosFocalDistInfo&& other);
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
    ~LocalTableData(){
//        delete[] m_data;
    }
public:
    SingleTableData m_mainPosTableData;       //正副表
    SingleTableData m_secondaryPosTableData;
    SingleTableData m_dbAngleDampingTableData;  //DB角度衰减表
    SingleTableData m_xyDistTableData;          //焦距参数表
    SingleTableData m_focalLengthMotorPosMappingData;    //光斑在不同焦距对应的电机焦距步  25种焦距 6种光斑
//    int* m_data;
    QSharedPointer<int> m_data;
    const int dataLen=(31*31*3*2+46+25*2+25*6)*sizeof(int);     //正副表，DB角度衰减表，焦距参数表
};

#endif // MODELS_H
