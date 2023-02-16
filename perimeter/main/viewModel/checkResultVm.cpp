#include "checkResultVm.h"
#include "paramsVm.h"
#include <QImage>
#include <QPainter>
namespace Perimeter
{

StaticCheckResultVm::StaticCheckResultVm(const QVariantList &args)
{
    if(args.count()==1)
    {
        int checkResult_id=args[0].toInt();
        CheckResult_ptr checkResult_ptr(new CheckResult());
        checkResult_ptr->m_id=checkResult_id;
        qx::dao::fetch_by_id(checkResult_ptr);
        m_data.reset(new StaticCheckResultModel(checkResult_ptr));
        CheckResultVm::setData(m_data.data());

        m_params.reset(new StaticParamsVM(&m_data->m_params));
        m_resultData.reset(new StaticResultDataVm(&m_data->m_data));
    }
    else
    {
        m_data.reset(new StaticCheckResultModel());
        CheckResultVm::setData(m_data.data());
        m_params.reset(new StaticParamsVM(&m_data->m_params));
        m_resultData.reset(new StaticResultDataVm(&m_data->m_data));
    }
}

void StaticCheckResultVm::insert()
{
    auto sp=m_data->ModelToDB();
    sp->m_time=QDateTime::currentDateTime();
    qx::dao::insert(sp);
    m_data->m_id=sp->m_id;
    qDebug()<<sp->m_id;
}

void StaticCheckResultVm::update()
{
    auto sp=m_data->ModelToDB();
    qx::dao::update(sp);
}

int StaticCheckResultVm::drawRealTimeEyePosPic(int index)
{
    auto blob=m_data->m_blob;
    auto realTimeDB=m_data->m_data.realTimeDB;
    if(uint(index)>=realTimeDB.size()) return 0;
    int picIndexStart=0;
    for(int i=0;i<index;i++)
    {
        picIndexStart+=realTimeDB[i].size();
    }
//    int picCount=blob.size()/(320*240);
    for(uint i=0;i<realTimeDB[index].size();i++)
    {
        int picIndex=picIndexStart+i;
        auto qa=blob.mid(picIndex*320*240,320*240);
        QImage img((uchar*)qa.data(),320,240,QImage::Format_Grayscale8);
        img.save(R"(./realTimeEyePosPic/)"+QString::number(i)+".bmp");

    }
    return realTimeDB[index].size();
}

//void StaticCheckResultVm::drawPic(int index)
//{
////    auto picData=m_data->m_data.pic[index];
////    for(uint i=0;i<picData.size();i++)
////    {
////        auto db=std::stoi(picData[i][0]);
////        auto base64Str=picData[i][1].c_str();
////        QByteArray byteArr=QByteArray::fromStdString(base64Str);
////        QImage pic((const uchar *)byteArr.data(),320,240,QImage::Format_Grayscale8);
////        QPainter painter(&pic);
////        QFont font("consolas");
////        font.setPixelSize(14);
////        painter.setFont(font);
////        painter.drawText(QPoint{300,220},QString::number(db)+"db");
////        pic.save("./realTimeEye/"+QString::number(i)+".bmp");
////    }
//}

DynamicCheckResultVm::DynamicCheckResultVm(const QVariantList &args)
{
    if(args.count()==1)
    {
        int checkResult_id=args[0].toInt();
        CheckResult_ptr checkResult_ptr(new CheckResult());
        checkResult_ptr->m_id=checkResult_id;
        qx::dao::fetch_by_id(checkResult_ptr);
        m_data.reset(new DynamicCheckResultModel(checkResult_ptr));
        CheckResultVm::setData(m_data.data());
        m_params.reset(new DynamicParamsVM(&m_data->m_params));
        m_resultData.reset(new DynamicResultDataVm(&m_data->m_data));
    }
    else
    {
        m_data.reset(new DynamicCheckResultModel());
        CheckResultVm::setData(m_data.data());
        m_params.reset(new DynamicParamsVM(&m_data->m_params));
        m_resultData.reset(new DynamicResultDataVm(&m_data->m_data));
    }

}

void DynamicCheckResultVm::insert()
{
    auto sp=m_data->ModelToDB();
    qx::dao::insert(sp);
}

void DynamicCheckResultVm::update()
{
    auto sp=m_data->ModelToDB();
    qx::dao::update(sp);
}


}
