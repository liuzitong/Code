#ifndef TESTCLASS_H
#define TESTCLASS_H
#include "perimeter/base/common/perimeter_def.h"
#include <QByteArray>
#include <QDebug>
namespace Perimeter {
struct Folder
{
    QString name;
    QVector<QString> files;
    QVector<Folder> folders;

    void traverse()
    {
        traverseDummmy(*this);
    }

    void traverseDummmy(Folder folder)
    {
        qDebug()<<folder.name;
        qDebug()<<folder.files;
        for(auto &i:folder.folders)
        {
           traverseDummmy(i);
        }
    }

};

class TestClass:public QObject
{
    Q_OBJECT
public:
    TestClass()=default;
    ~TestClass()=default;
    Q_INVOKABLE void test();
    void createPatientData();
    void createCheckResultData();

    void createDynamicCheckResultData();
    void createCheckResultVm();
    void TestReport();
    void makePicData();
    void makePic();
    void drawPicData();
    void serialization();
    void testData();
    Folder getFileListUnderDir(const QString &dirPath);
};
}

#endif // TESTCLASS_H
