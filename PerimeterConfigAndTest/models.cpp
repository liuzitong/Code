#include "models.h"
#include <memory>


LocalTableData::LocalTableData()
{
    m_data=QSharedPointer<int>(new int[dataLen]{0});
    memset(m_data.data(),-1,dataLen*sizeof (int));
    m_mainPosTableData=SingleTableData(31*3,31,m_data.data());
    m_secondaryPosTableData=SingleTableData(31*3,31,m_data.data()+31*3*31);
    m_dbAngleDampingTableData=SingleTableData(46,1,m_data.data()+31*3*31*2);
    m_xyDistTableData=SingleTableData(25,2,m_data.data()+31*3*31*2+46);
    m_focalLengthMotorPosMappingData=SingleTableData(25,6,m_data.data()+31*3*31*2+46+25*2);
}

CoordMotorPosFocalDistInfo::CoordMotorPosFocalDistInfo(int x, int y, int f):motorX(x),motorY(y),focalDist(f)
{

}

CoordMotorPosFocalDistInfo &CoordMotorPosFocalDistInfo::operator=(CoordMotorPosFocalDistInfo& other)
{
    this->motorX=other.motorX;
    this->motorY=other.motorY;
    this->focalDist=other.focalDist;
    return *this;
}

CoordMotorPosFocalDistInfo &CoordMotorPosFocalDistInfo::operator=(CoordMotorPosFocalDistInfo&& other)
{
    this->motorX=other.motorX;
    this->motorY=other.motorY;
    this->focalDist=other.focalDist;
    return *this;
}

