#ifndef DICOM_H
#define DICOM_H

#include <QPair>
#include <QString>
#include <QVector>
// #include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <perimeter/main/model/patientmodel.h>
// #include <dcmtk/dcmdata/dcistrmf.h>
// #include <dcmtk/dcmdata/libi2d/i2dbmps.h>

class Dicom
{
public:
    Dicom();
    static Dicom* getSingleton();
    bool upLoadDcm(PatientModel patientModel);
private:
    QString cmdPdfToDcm(QVector<QPair<DcmTagKey,QString>> infos);
    static Dicom* singleton;
    QString pdfFilePath="dicom.pdf";
    QString dcmFilePath="dicom.dcm";
    QString serverIP;
    QString serverPort;
    QString aec;
    QString aet;

};



#endif // DICOM_H
