#include "signatureHelper.h"
#include <QFile>
#include <QImage>
bool SignatureHelper::upLoadSignature(QString sourceFilePath,int UID)
{
    // QFile file(sourceFilePath);
    QImage img(sourceFilePath);
    QString destFilePath="./signature/"+QString::number(UID)+".png";
    // file.copy("./signature/"+QString::number(UID)+".PNG");

    // if(QFile::exists(destFilePath))
    //     QFile::remove(destFilePath);

    // return QFile::copy(sourceFilePath, destFilePath);
    return img.save(destFilePath);
}
