#include "visionFieldIslandVm.h"
#include <perimeter/main/model/checkResultModel.h>
#include <perimeter/main/model/patientmodel.h>
#include <perimeter/main/services/analysis_svc.h>
#include <QVariantList>
#include <qimage.h>

namespace Perimeter{


VisionFieldIslandVm::VisionFieldIslandVm(const QVariantList &args)
{
    int resultId=args[0].toInt();

    CheckResult_ptr checkResult_ptr(new CheckResult());
    checkResult_ptr->m_id=resultId;
    qx::dao::fetch_by_id(checkResult_ptr);
    Program_ptr program_ptr(new Program());
    program_ptr->m_id=checkResult_ptr->m_program->m_id;
    qx::dao::fetch_by_id(program_ptr);
    Patient_ptr patient_ptr(new Patient());
    patient_ptr->m_id=checkResult_ptr->m_patient->m_id;
    qx::dao::fetch_by_id(patient_ptr);

    QVector<int> values;
    QVector<QPointF> locs;

    auto checkResult=StaticCheckResultModel(checkResult_ptr);
    auto program=StaticProgramModel(program_ptr);
    values.resize(checkResult.m_data.checkData.size());
    for(int i=0;i<int(checkResult.m_data.checkData.size());i++)
    {
        values[i]=checkResult.m_data.checkData[i];
    }
    locs.resize(program.m_data.dots.size());
    for(int i=0;i<int(program.m_data.dots.size());i++)
    {
        locs[i]={program.m_data.dots[i].x,program.m_data.dots[i].y};
    }
    auto innerRange=program.m_params.commonParams.Range[0];
    auto range=program.m_params.commonParams.Range[1];

    QImage img_vison=QImage({range*2,range*2}, QImage::Format_RGB32);
    auto analysisMethodSvc=AnalysisSvc::getSingleton();
    analysisMethodSvc->drawVisionFieldIsland(values,locs,range,innerRange,img_vison);
    QImage img_vison2=QImage({int(range*1.2*2),int(range*1.2*2)}, QImage::Format_RGB32);
    img_vison2.fill(qRgb(0,0,0));
    QPainter painter(&img_vison2);
    painter.drawImage(QPoint{(img_vison2.width()-img_vison.width())/2,(img_vison2.height()-img_vison.height())/2},img_vison);
    img_vison2.save(QString("./visionFieldIsland/")+"visionField.png");
}

VisionFieldIslandVm::~VisionFieldIslandVm()
{

}
}
