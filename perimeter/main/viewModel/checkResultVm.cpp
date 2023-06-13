#include "checkResultVm.h"
#include "paramsVm.h"
#include <QImage>
#include <QPainter>
#include <deviceOperation/device_operation.h>
#include <QtMath>
#include "perimeter/main/services/utility_svc.h"
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

StaticCheckResultVm::StaticCheckResultVm(const CheckResult_ptr & checkResult_ptr)
{
    m_data.reset(new StaticCheckResultModel(checkResult_ptr));
    CheckResultVm::setData(m_data.data());
    m_params.reset(new StaticParamsVM(&m_data->m_params));
    m_resultData.reset(new StaticResultDataVm(&m_data->m_data));
}

void StaticCheckResultVm::insert()
{
    qDebug()<<"insert result";
    auto sp=m_data->ModelToDB();
    sp->m_time=QDateTime::currentDateTime();
    auto realTimeEyePosPicSize=Perimeter::UtilitySvc::getSingleton()->m_realTimeEyePosPicSize;
    auto imgSize=/*DevOps::DeviceOperation::getSingleton()->m_videoSize;*/m_data->m_videoSize;
    qx::dao::insert(sp);
//    qDebug()<<sp->m_id;
    m_data->m_id=sp->m_id;
    for(int i=0;i<m_data->m_imgData.length();i++)                                //图片转换位设置大小并且存储
    {
        auto& dotImgDatas=m_data->m_imgData[i];
        for(int j=0;j<dotImgDatas.length();j++)
        {
            auto& imgData=dotImgDatas[j];
            if(imgData.size()>0)
            {
                QImage img((uchar*)imgData.data(),imgSize.width(),imgSize.height(),QImage::Format_Grayscale8);
                img=img.scaled(realTimeEyePosPicSize.width(),realTimeEyePosPicSize.height());

                QString fileDir=R"(./savePics/)"+
                        QString::number(qCeil(m_data->m_patient_id/100))+"/"+
                        QString::number(m_data->m_patient_id)+"/"+
                        QString::number(m_data->m_id)+"/"+
                        QString::number(i)+"/";

                QDir dir;
                if(!dir.exists(fileDir))
                {
                    dir.mkpath(fileDir);
                }
                qDebug()<<fileDir;
                img.save(fileDir+QString::number(j)+".JPEG");
//                const char* data=(char*)(img.bits());
//                QByteArray ba(data,realTimeEyePosPicSize.width()*realTimeEyePosPicSize.height());
//                sp->m_blob.append(ba);
            }
        }
    }

}

void StaticCheckResultVm::update()
{
    auto sp=m_data->ModelToDB();
    qx::dao::update(sp);
}

QVariantList StaticCheckResultVm::drawRealTimeEyePosPic(int index)
{
    QVariantList list;
    auto realTimeDB=m_data->m_data.realTimeDB;
    auto imgSize=/*DevOps::DeviceOperation::getSingleton()->m_videoSize;*/m_data->m_videoSize;
    if(uint(index)>=realTimeDB.size()) return {0};
    int picIndexStart=0;
    for(int i=0;i<index;i++)
    {
        picIndexStart+=realTimeDB[i].size();
    }
    if(m_data->m_imgData.size()>0)                    //检查的时候
    {
        auto imgs=m_data->m_imgData[index];
        if(imgs.size()>0)
        {
            for(int i=0;i<imgs.length();i++)
            {
                QImage img((uchar*)imgs[i].data(),imgSize.width(),imgSize.height(),QImage::Format_Grayscale8);
                img.save(R"(./realTimeEyePosPic/)"+QString::number(i)+".JPEG");
            }
        }
        return {imgs.size(),QString(R"(./realTimeEyePosPic/)")};
    }
    else                                            //读取结果的时候
    {
//        auto blob=m_data->m_blob;
//        if(blob.size()==0) return 0;
//        for(uint i=0;i<realTimeDB[index].size();i++)
//        {
//            int picIndex=picIndexStart+i;
//            if(picIndex*imgSize.width()*imgSize.height()+imgSize.width()*imgSize.height()>blob.size()) return 0;
//            auto qa=blob.mid(picIndex*imgSize.width()*imgSize.height(),imgSize.width()*imgSize.height());
//            QImage img((uchar*)qa.data(),imgSize.width(),imgSize.height(),QImage::Format_Grayscale8);
//            img.save(R"(./realTimeEyePosPic/)"+QString::number(i)+".bmp");

//        }
        QString fileDir=R"(/savePics/)"+
                QString::number(qCeil(m_data->m_patient_id/100))+"/"+
                QString::number(m_data->m_patient_id)+"/"+
                QString::number(m_data->m_id)+"/"+
                QString::number(index)+"/";
        qDebug()<<fileDir;
        return {realTimeDB[index].size(),fileDir};
    }
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
        m_data->m_type=Type::Dynamic;                                           //不然界面刷新会遇到问题
        CheckResultVm::setData(m_data.data());
        m_params.reset(new DynamicParamsVM(&m_data->m_params));
        m_resultData.reset(new DynamicResultDataVm(&m_data->m_data));
    }

}

DynamicCheckResultVm::DynamicCheckResultVm(const CheckResult_ptr &checkResult_ptr)
{
    m_data.reset(new DynamicCheckResultModel(checkResult_ptr));
    CheckResultVm::setData(m_data.data());
    m_params.reset(new DynamicParamsVM(&m_data->m_params));
    m_resultData.reset(new DynamicResultDataVm(&m_data->m_data));
}

void DynamicCheckResultVm::insert()
{
    auto sp=m_data->ModelToDB();
    sp->m_time=QDateTime::currentDateTime();
    qx::dao::insert(sp);
    m_data->m_id=sp->m_id;
    qDebug()<<sp->m_id;
}

void DynamicCheckResultVm::update()
{
    auto sp=m_data->ModelToDB();
    qx::dao::update(sp);
}


}
