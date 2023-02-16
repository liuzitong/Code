#include "testclass.h"

#include <QJsonArray>
#include <qfile.h>
#include <qjsondocument.h>
#include <QJsonObject>
#include <perimeter/main/model/checkResultModel.h>
#include <perimeter/main/viewModel/checkResultVm.h>
#include "perimeter/main/viewModel/settings.h"
#include "perimeter/main/Model/utility.h"
#include "perimeter/main/viewModel/progressAnalysislistVm.h"
#include <perimeter/main/services/analysis_svc.h>
#include "qxpack/indcom/common/qxpack_ic_global.hxx"


namespace Perimeter {


void TestClass::test()
{
//    createPatientData();
//    createCheckResultData();
//    createDynamicCheckResultData();
//    createCheckResultVm();
//    TestReport();
//    makePicData();
//    makePic();
//    drawPicData();
//testData();
//    serialization();

}

void TestClass::createPatientData()
{

    QxPack::IcLCG  lcg;
    Patient_ptr patient_1;
//    QSqlDatabase db = qx::QxSqlDatabase::getDatabase();
//    bool bCommit = db.transaction();
    QList<Patient_ptr> pplist;
    for(int i=0;i<2000;i++)
    {
        Patient_ptr patient_ptr;
        QDateTime time = QDateTime::currentDateTime();
        time = time.addDays(-lcg.value()%365);
        time = time.addMonths(-lcg.value()%12);
        time = time.addYears(-lcg.value()%80);
        QString nm = QString("%1%2%3").arg( QChar( ( lcg.value() % 26 ) + 'A' ))
                         .arg( QChar( ( lcg.value() % 26 ) + 'a' ))
                         .arg( QChar( ( lcg.value() % 26 ) + 'a' ));
        QString id = QString("PID_%1").arg( lcg.value() % 5000 );


    //        Patient::sex sex=Patient::sex::male;
    //        pp.reset(new Patient("998","fdsfds",sex,QDate::fromString("1988-05-11","yyyy-MM-dd")));
        auto pp=new Patient(id,nm,sex(lcg.value()%2),time.date(),
                  R"(<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
                    <!DOCTYPE boost_serialization>
                    <boost_serialization signature="serialization::archive" version="19">
                    <t class_id="0" tracking_level="0" version="0">
                        <rx1_r>6.000000238e-01</rx1_r>
                        <rx2_r>6.999999881e-01</rx2_r>
                        <rx3_r>8.000000119e-01</rx3_r>
                        <visual_r>8.999999762e-01</visual_r>
                        <rx1_l>2.000000030e-01</rx1_l>
                        <rx2_l>3.000000119e-01</rx2_l>
                        <rx3_l>4.000000060e-01</rx3_l>
                        <visual_l>5.000000000e-01</visual_l>
                    </t>
                    </boost_serialization>)"
                    );
        pplist.append(patient_ptr);
    }
//    qDebug()<<list.length();
//    qx::dao::insert(list);
    qx::dao::insert(pplist);

    //    db.commit();
}

void TestClass::createCheckResultData()
{
    StaticCheckResultModel checkModel;
    checkModel.m_type=Type::ThreshHold;
    checkModel.m_OS_OD=0;
    checkModel.m_diagnosis="static";
    checkModel.m_time=QDateTime::currentDateTime();

    checkModel.m_params.commonParams={{1,2}};
    checkModel.m_params.fixedParams={1,2};
    checkModel.m_data.checkData={55,2};
    checkModel.m_data.alarm=0;
    checkModel.m_data.pupilDiameter=5.3;
    checkModel.m_data.testTimespan=100;
    checkModel.m_data.fixationDeviation={1,2,3,4};
    checkModel.m_data.falseNegativeCount=1;
    checkModel.m_data.checkData={2,3,4,5};
//    checkModel.m_data.realTimeDB={{22},{24,23},{32,25,14},{32,25,14,44},{13,25,14,32,21},{13,25,44,32,26,42},{13,25,14,32,21},{32,25,14,44},{32,25,14},{24,23},{22}};

    auto pp=checkModel.ModelToDB();
    qx::dao::insert(pp);

}

void TestClass::createDynamicCheckResultData()
{
    DynamicCheckResultModel checkModel;
    checkModel.m_type=Type::Dynamic;
    checkModel.m_OS_OD=0;
    checkModel.m_diagnosis="Dynamic";
    checkModel.m_time=QDateTime::currentDateTime();
    checkModel.m_params={{2,3},DynamicParams::Strategy::darkArea,CursorColor::white};
    checkModel.m_data.alarm=0;
    checkModel.m_data.pupilDiameter=5.3;
    checkModel.m_data.testTimespan=100;
    checkModel.m_data.fixationDeviation={1,2,3,4};
    checkModel.m_data.checkData={{"A",{50,45},{50,45},true},{"B",{50,135},{50,135},true},{"C",{50,225},{50,225},true},{"D",{50,315},{50,315},true}};



}

void TestClass::createCheckResultVm()
{
    StaticCheckResultVm resultVm({1});;
    qDebug()<<resultVm.getResultData()->getCheckData()[2];

}

void TestClass::TestReport()
{
    auto reportEngine = QSharedPointer<LimeReport::ReportEngine>(new LimeReport::ReportEngine());

    reportEngine->loadFromFile("./reports/demo.lrxml");
    auto manager=reportEngine->dataManager();
    auto dataSource=manager->dataSource("od_model");
    auto model=dataSource->model();
    model->insertRows(0,4);
    for(int i=0;i<4;i++)
    {
        model->setData( model->index(i,0),"wawa");
        model->setData( model->index(i,1),"wawa1");
        model->setData( model->index(i,2),"wawa2");
        model->setData( model->index(i,3),"wawa2");
        model->setData( model->index(i,4),"wawa2");
        model->setData( model->index(i,5),"wawa2");
    }

//    qDebug()<<model->data(QModelIndex( model->index(0,0)));
//    QStringList simpleData;
//    simpleData << "value1" << "value2" << "value3";
//    QStringListModel* stringListModel = new QStringListModel();
//    stringListModel->setStringList(simpleData);
//    manager->addModel("bba.3",stringListModel,true);
    reportEngine->setShowProgressDialog(true);
    reportEngine->setPreviewScaleType(LimeReport::ScaleType::Percents,50);
    reportEngine->previewReport(/*LimeReport::PreviewHint::ShowAllPreviewBars*/);
}

void TestClass::makePicData()
{
    StaticCheckResultVm checkResultVm(QVariantList{1});
    auto resultModel=checkResultVm.getModel();
    resultModel->m_data.realTimeDB={{22},{24,23},{32,25,14},{32,25,14,44},{13,25,14,32,21},{13,25,44,32,26,42},{13,25,14,32,21},{32,25,14,44},{32,25,14},{24,23},{22}};

    QByteArray byteArr;
    byteArr.fill(uchar(255),320*240);
    auto dbData=resultModel->m_data.realTimeDB;
    qDebug()<<dbData.size();
    for(quint8 i=0;i<dbData.size();i++)
    {
        int number=0;
        for(quint8 j=0;j<dbData[i].size();j++)
        {
            number++;
            for(int h=0;h<240;h++)
            {
                for(int w=0;w<320;w++)
                {
                    byteArr.data()[h*320+w]=w*number%255;
                }
            }
            resultModel->m_blob.append(byteArr);
        }
    }
    auto pp=resultModel->ModelToDB();
    qx::dao::update(pp);


}

void TestClass::makePic()
{

    QByteArray byteArr;
    byteArr.fill(uchar(255),320*240);
    for(int h=0;h<240;h++)
    {
        for(int w=0;w<320;w++)
        {
            byteArr.data()[h*320+w]=w%255;
        }
    }
    QImage img((uchar*)byteArr.data(),320,240,QImage::Format_Grayscale8);
    img.save(R"(C:\Users\syseye\Desktop\Pic\generated.bmp)");
}

void TestClass::drawPicData()
{
    StaticCheckResultVm checkResultVm(QVariantList{1});
    auto resultModel=checkResultVm.getModel();
    auto pic=resultModel->m_blob;
    int picCount=pic.size()/(320*240);
    qDebug()<<picCount;
    for(int i=0;i<picCount;i++)
    {
        auto qa=pic.mid(i*320*240,320*240);
        QImage img((uchar*)qa.data(),320,240,QImage::Format_Grayscale8);
        img.save(R"(C:\Users\syseye\Desktop\Pic\fromModel\)"+QString::number(i)+".bmp");
    }
//    auto byteArr=QByteArray::fromBase64(base64String.c_str());

}

void TestClass::serialization()
{
//    std::vector<std::vector<uint8_t[320*240]>> pic;
//    for(int i=0;i<3;i++)
//    {
//        std::vector<uint8_t[320*240]> picA;
//        for(int j=0;j<6;j++)
//        {
//            uint8_t bytes[320*240];
//            memset(&bytes[0],i*j,320*240);
//            picA.push_back(bytes);
//        }
//        pic.push_back(picA);
//    }
    //    qDebug()<<Utility::entityToQString(pic);
}

void TestClass::testData()
{
    CheckResult_ptr checkResult_ptr(new CheckResult());
    checkResult_ptr->m_id=331;
    qx::dao::fetch_by_id(checkResult_ptr);

    auto blob=checkResult_ptr->m_blob;
    for(int i=0;i<4;i++)
    {
        qDebug()<<QString::number(blob.data()[i]);
    }
}




Folder TestClass::getFileListUnderDir(const QString &dirPath)
{

    Folder folder;
//    QStringList fileList;
    QDir dir(dirPath);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
    folder.name=dir.path();
    foreach (auto fileInfo, fileInfoList) {
        if(fileInfo.isDir())
        {
            folder.folders.append(getFileListUnderDir(fileInfo.absoluteFilePath()));
        }

        if(fileInfo.isFile())
        {
            qDebug() << __FUNCTION__ << __LINE__ << "  : " << fileInfo.absoluteFilePath();
            folder.files.append( fileInfo.absoluteFilePath());
        }
    }
    return folder;
//    return fileList;
}


}
