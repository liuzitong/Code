#ifndef PROGRAMVM_H
#define PROGRAMVM_H
#include <QObject>
#include <perimeter/main/model/programModel.h>
#include <perimeter/main/viewModel/paramsVm.h>
#include <QVector>
#include <QPoint>
#include <perimeter/main/database/program.h>
#include <qsharedpointer.h>
#include <vector>
#include <perimeter/main/appctrl/perimeter_appctrl.hxx>
#include <perimeter/third-part/qxpack/indcom/ui_qml_base/qxpack_ic_ui_qml_api.hxx>
#include <perimeter/main/viewModel/settings.h>
namespace Perimeter
{

class StaticProgramDataVm:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList strategies READ getStrategies WRITE setStrategies NOTIFY strategiesChanged)
    Q_PROPERTY(QVariantList strategyModes READ getStrategyModes)
    Q_PROPERTY(QVariantList baseDots READ getBaseDots)
    Q_PROPERTY(QVariantList dots READ getDots WRITE setDots NOTIFY dotsChanged)                     //data

public:
    QVariantList getStrategies()
    {
        QVariantList strategies;
        for(auto&i:m_data->strategies)
        {
            strategies.append(int(i));
        }
        return strategies;
    }
    void setStrategies(QVariantList value)
    {
//       m_data->strategies.clear();
//       for(auto&i:value)
//       {
//           m_data->strategies.push_back(StaticParams::CommonParams::Strategy(i.toInt()));
//       }
        std::vector<int> strategies;
        for(auto&i:value)
        {
            strategies.push_back(i.toInt());
        }

        std::sort(strategies.begin(),strategies.end(),[](int a,int b)->bool{return a<b;});


        m_data->strategies.clear();
        for(auto&i:strategies)
        {
            m_data->strategies.push_back(StaticParams::CommonParams::Strategy(i));
        }

        strategiesChanged();
    }

    Q_SIGNAL void strategiesChanged();

    QVariantList getStrategyModes()
    {
        QVariantList strategyModes;
        for(auto&i:m_data->strategyModes)
        {
            strategyModes.append(int(i));
        }
        return strategyModes;
    }

    QVariantList getBaseDots()
    {
        QVariantList baseDots;
        for(auto&i:m_data->baseDots)
        {
            baseDots.append(QPointF{i.x,i.y});
        }
        return baseDots;
    }

    QVariantList getDots()
    {
        QVariantList dots;
        for(auto&i:m_data->dots)
        {
            dots.append(QPointF{i.x,i.y});
        }
        return dots;
    }

    void setDots(QVariantList value)
    {
        m_data->dots.clear();
        for(auto& v:value)
        {
            if(!v.canConvert(QMetaType::QPointF))
            {
                float x=v.toMap()["x"].toFloat();
                float y=v.toMap()["y"].toFloat();
                m_data->dots.push_back({x,y});
            }
            else{
                m_data->dots.push_back({float(v.toPointF().x()),float(v.toPointF().y())});
            }
        }
        dotsChanged();
    }

    Q_SIGNAL void dotsChanged();

public:
    StaticProgramDataVm(StaticProgramData* data){m_data=data;}
private:
    StaticProgramData* m_data;
};

class DynamicProgramDataVm:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList dots READ getDots WRITE setDots )                     //data

    QVariantList getDots()
    {
        QVariantList dots;
        for(auto&i:m_data->dots)
        {
            dots.append(QPointF{i.x,i.y});
        }
        return dots;
    }

    void setDots(QVariantList value)
    {
        m_data->dots.clear();
        for(auto& v:value)
        {
            if(!v.canConvert(QMetaType::QPointF))
            {
                float x=v.toMap()["x"].toFloat();
                float y=v.toMap()["y"].toFloat();
                m_data->dots.push_back({x,y});
            }
            else{
                m_data->dots.push_back({float(v.toPointF().x()),float(v.toPointF().y())});
            }
        }
    }
public:
    DynamicProgramDataVm(DynamicProgramData* data){m_data=data;}
private:
    DynamicProgramData* m_data;
};


class ProgramVm:public QObject
{
    Q_OBJECT
    Q_PROPERTY(long id READ getID WRITE setID)
    Q_PROPERTY(int type READ getType WRITE setType NOTIFY typeChanged())
    Q_PROPERTY(QString name READ getName WRITE setName);
    Q_PROPERTY(QVariantList report READ getReport WRITE setReport )
    Q_PROPERTY(int category READ getCategory WRITE setCategory NOTIFY categoryChanged)
public:
    long getID(){return m_data->m_id;}void setID(int value){m_data->m_id=value;}
    long getType(){return long(m_data->m_type);}void setType(int value){m_data->m_type=Type(value);typeChanged();}Q_SIGNAL void typeChanged();
    QString getName(){
        auto name= m_data->m_name;

//        auto splitNames=name.split("@");
//        if(splitNames.count()!=2)
//        {
//            return name;
//        }
//        else
//        {
//            auto appCtrl=static_cast<AppCtrl*>(QxPack::IcUiQmlApi::appCtrl());
//            auto setting=static_cast<Settings*>(appCtrl->getSettings());
//            auto lang=setting->getLanguage();
//            if(lang=="Chinese"||(lang=="Default"&&QLocale::system().language()==QLocale::Chinese))
//                return splitNames[1];
//            else
//                return splitNames[0];
//        }
         return TranslateController::getTranlatedName(name);
    }
    void setName(QString value){m_data->m_name=value;}
    QVariantList getReport()
    {
        QVariantList reports;
        for(auto&i: m_data->m_report)
        {
            reports.append(i);
        }
        return reports;
    }
    void setReport(QVariantList value)
    {
        m_data->m_report.clear();
        for(auto&i:value)
        {
            m_data->m_report.push_back(i.toInt());
        }
    }
    int getCategory(){return int(m_data->m_category);}void setCategory(int value){m_data->m_category=Category(value);categoryChanged();}Q_SIGNAL void categoryChanged();
protected:
    ProgramModel* m_data;
};

class StaticProgramVm:public ProgramVm
{
    Q_OBJECT
    Q_PROPERTY(QObject* params READ getParams)
    Q_PROPERTY(QObject* data READ getData)
public:
    Q_INVOKABLE explicit StaticProgramVm(const QVariantList &);
    Q_INVOKABLE virtual ~StaticProgramVm()=default;
    Q_INVOKABLE void updateProgram();
    Q_INVOKABLE void insertProgram();
    Q_INVOKABLE void deleteProgram();
    Q_INVOKABLE void circleDots(int outterRadius,int innerRadius,int gap);
    Q_INVOKABLE void rectangleDots(int x0,int y0,int x1,int y1,int gap);


    StaticParamsVM* getParams(){return m_staticParamsVm.data();}
    StaticProgramDataVm* getData(){return m_staticDataVm.data();}

    QSharedPointer<StaticProgramModel> getModel(){return m_data;}
private:
    QSharedPointer<StaticProgramModel> m_data;
    QSharedPointer<StaticParamsVM> m_staticParamsVm;
    QSharedPointer<StaticProgramDataVm> m_staticDataVm;

};


class DynamicProgramVm:public ProgramVm
{
    Q_OBJECT
    Q_PROPERTY(QObject* params READ getParams);
    Q_PROPERTY(QObject* data READ getData)

public:
    Q_INVOKABLE explicit DynamicProgramVm(const QVariantList &);
    Q_INVOKABLE virtual ~DynamicProgramVm()=default;
    Q_INVOKABLE void updateProgram();
    Q_INVOKABLE void insertProgram();
    Q_INVOKABLE void deleteProgram();

    DynamicParamsVM* getParams(){return m_dynamicParamsVm.data();}
    DynamicProgramDataVm* getData(){return m_dynamicDataVm.data();}

    QSharedPointer<DynamicProgramModel> getModel(){return m_data;}
private:
    QSharedPointer<DynamicProgramModel> m_data;
    QSharedPointer<DynamicParamsVM> m_dynamicParamsVm;
    QSharedPointer<DynamicProgramDataVm> m_dynamicDataVm;
};
}


#endif // PROGRAMVM_H
