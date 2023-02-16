#ifndef ANALYSISLOBBYLISTVM_H
#define ANALYSISLOBBYLISTVM_H
#include <QAbstractListModel>
#include <perimeter/main/services/analysis_svc.h>

namespace Perimeter{
class AnalysisLobbyListVm:public QAbstractListModel
{
    enum LobbyAnalysisRoles
    {
        year=Qt::UserRole+1,
        monthDay,
        simpleCheckResult,
    };

    struct Data
    {
        QString year;
        QString monthDay;
        QObject* simpleCheckResult=nullptr;
    };

    Q_OBJECT
public:
    AnalysisLobbyListVm(){}
    Q_INVOKABLE explicit AnalysisLobbyListVm(const QVariantList &args);
    Q_INVOKABLE virtual ~AnalysisLobbyListVm() Q_DECL_OVERRIDE;
    Q_INVOKABLE void deleteCheckResult(int id);
    Q_INVOKABLE void refreshData();

public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
private:
    void generateModelListData(CheckResult_List checkResult_list);
    QString drawImage(CheckResult_ptr checkResult_ptr);

    QVector<Data> m_dataList;
    float m_imageHeight;
    int m_imageSerial=0;
};
}

#endif // ANALYSISLOBBYLISTVM_H
