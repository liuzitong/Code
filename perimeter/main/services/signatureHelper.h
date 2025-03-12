#ifndef SIGNATUREHELPER_H
#define SIGNATUREHELPER_H

#include <QObject>
class SignatureHelper:public QObject
{
Q_OBJECT
public:
    SignatureHelper()=default;
    Q_INVOKABLE bool upLoadSignature(QString sourceFilePath,int UID);
};

#endif // SIGNATUREHELPER_H
