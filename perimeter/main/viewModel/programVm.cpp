#include "programVm.h"
#include <qDebug>
#include <math.h>
namespace Perimeter {

StaticProgramVm::StaticProgramVm(const QVariantList &args)
{
    StaticProgramModel* pp;
    if(args.count()==0)
    {
        pp=new StaticProgramModel;
        using spcp=StaticParams::CommonParams;
        pp->m_params={StaticParams::CommonParams{{0,30},spcp::Strategy::fastInterative,spcp::StrategyMode::ageRelated,CursorColor::white,CursorSize::I,BackGroundColor::white,
                                                 true,true,850,true,true,FixationTarget::centerPoint,FixationMonitor::dontAlarm,true},
                      StaticParams::FixedParams{180,1000,20,20,10,10,10,10}};
    }
    else
    {
//       int program_id=args[0].toInt();
//       qx_query query("select * from program where program_id=:program_id");
//       query.bind(":program_id",program_id);
//       Program_List program_List;
//       QSqlError daoError = qx::dao::execute_query(query, program_List);
//       Program_ptr program_ptr=program_List.first();

        Program_ptr program_ptr(new Program);
        qDebug()<<args[0].toInt();
        program_ptr->m_id=args[0].toInt();
        qx::dao::fetch_by_id(program_ptr);
        pp=new StaticProgramModel(program_ptr);
    }
    m_data.reset(pp);
    ProgramVm::m_data=pp;
    m_staticDataVm.reset(new StaticProgramDataVm(&m_data->m_data));
    m_staticParamsVm.reset(new StaticParamsVM(&m_data->m_params));
    qDebug()<<m_staticParamsVm->getFixedParams()->getStimulationTime();
}

void StaticProgramVm::updateProgram()
{
    auto pp=m_data->ModelToDB();
    qx::dao::update(pp);
}

void StaticProgramVm::insertProgram()
{
    auto pp=m_data->ModelToDB();
    qx::dao::insert(pp);
}

void StaticProgramVm::deleteProgram()
{
    Program_ptr pp(new Program);
    pp->m_id=m_data->m_id;
    qx::dao::delete_by_id(pp);
}

void StaticProgramVm::circleDots(int outterRadius, int innerRadius, int gap)
{
//    std::vector<Point> dots;
    int grid=floor(double(outterRadius)/gap-0.5);
    int x_begin=-grid*gap-0.5*gap;
    int x_end=grid*gap+0.5*gap;
    int y_begin=x_end;
    int y_end=x_begin;
    for(int y=y_begin;y>=y_end;y=y-gap)
        for(int x=x_begin;x<=x_end;x+=gap)
        {
            int radius_squared=pow(y,2)+pow(x,2);
            if(radius_squared<=pow(outterRadius,2)&&radius_squared>=pow(innerRadius,2))
            {
                m_data->m_data.dots.push_back(Point{float(x),float(y)});
            }
        }
     m_staticDataVm->dotsChanged();
}

void StaticProgramVm::rectangleDots(int x0, int y0, int x1, int y1, int gap)
{

    int x_begin=floor(double(x0)/gap)*gap+0.5*gap;
    int x_end=ceil(double(x1)/gap)*gap-0.5*gap;
    int y_begin=floor(double(y0)/gap)*gap+0.5*gap;
    int y_end=ceil(double(y1)/gap)*gap-0.5*gap;
    for(int y=y_begin;y>=y_end;y=y-gap)
        for(int x=x_begin;x<=x_end;x+=gap)
        {
            m_data->m_data.dots.push_back(Point{float(x),float(y)});
        }
    m_staticDataVm->dotsChanged();
}

DynamicProgramVm::DynamicProgramVm(const QVariantList &args)
{
    DynamicProgramModel* pp;
    using dp=DynamicParams;
    if(args.count()==0)
    {
        pp=new DynamicProgramModel;
        pp->m_params={{0,30},dp::Strategy::standard,CursorColor::white,CursorSize::I,BackGroundColor::white,1,10,dp::DynamicMethod::_6Lines,dp::DynamicDistance::_10,FixationMonitor::onlyAlarm};
    }
    else
    {
        Program_ptr program_ptr(new Program);
        program_ptr->m_id=args[0].toInt();
        qx::dao::fetch_by_id(program_ptr);
        pp=new DynamicProgramModel(program_ptr);
    }
    ProgramVm::m_data=pp;
    m_data.reset(pp);
    m_dynamicDataVm.reset(new DynamicProgramDataVm(&m_data->m_data));
    m_dynamicParamsVm.reset(new DynamicParamsVM(&m_data->m_params));
}

void DynamicProgramVm::updateProgram()
{
    auto pp=m_data->ModelToDB();
    qx::dao::update(pp);
}

void DynamicProgramVm::insertProgram()
{
    auto pp=m_data->ModelToDB();
    qx::dao::insert(pp);
}

void DynamicProgramVm::deleteProgram()
{
    Program_ptr pp(new Program);
    pp->m_id=m_data->m_id;
    qx::dao::delete_by_id(pp);
}







//StaticProgramVM::StaticProgramVM(const QVariantList & args)
//{
//    if(args.count()==0)
//    {
//        m_staticParamsVm=new StaticParamsVM();
//        return;
//    }
////    qDebug()<<"static programvm cons";
//    int program_id=args[0].toInt();
////    qDebug()<<program_id;
//    qx_query query("select * from program where program_id=:program_id");
//    query.bind(":program_id",program_id);
//    Program_List program_List;
//    QSqlError daoError = qx::dao::execute_query(query, program_List);
//    Program_ptr program_ptr=program_List.first();
////    qDebug()<<program_ptr->m_params;
//    QSharedPointer<StaticProgramModel> programModel=QSharedPointer<StaticProgramModel>(new StaticProgramModel(program_ptr));
//    m_id=programModel->m_id;
//    m_type=int(programModel->m_type);
//    m_name=programModel->m_name;
//    m_staticParamsVm=new StaticParamsVM(programModel->m_params);
////    qDebug()<<programModel->m_params.commonParams.Range[0];
////    qDebug()<<programModel->m_params.commonParams.Range[1];
//    m_category=int(programModel->m_category);

//    for(auto& v:programModel->m_report)
//    {
//        m_report.append(v);
//    }

//    for(auto& v:programModel->m_data.strategies)
//    {
//        m_strategies.append(int(v));
//    }
////    qDebug()<<program_ptr->m_data;
//    for(auto& v:programModel->m_data.dots)
//    {
//        m_dots.append(QPointF(v.x,v.y));
//    }
//}

//StaticProgramVM::~StaticProgramVM()
//{
//    delete m_staticParamsVm;
//}

//void StaticProgramVM::updateProgram()
//{
//    Program_ptr pp=getProgramData();
//    QSqlError error=qx::dao::update(pp);
//}

//void StaticProgramVM::insertProgram()
//{
//    Program_ptr pp=getProgramData();
//    QSqlError error=qx::dao::insert(pp);
//}

//void StaticProgramVM::deleteProgram()
//{
//    qDebug()<<"delete Program";
//    Program_ptr pp=getProgramData();
//    qDebug()<<pp->m_id;
//    QSqlError error=qx::dao::delete_by_id(pp);
//}

//Program_ptr StaticProgramVM::getProgramData()
//{
//    QSharedPointer<StaticProgramModel> programModel=QSharedPointer<StaticProgramModel>(new StaticProgramModel());
//    programModel->m_id=m_id;
//    programModel->m_name=m_name;
//    programModel->m_type=Type(m_type);
//    programModel->m_params.commonParams=m_staticParamsVm->getCommonParams()->getData();
//    programModel->m_params.fixedParams=m_staticParamsVm->getFixedParams()->getData();
//    programModel->m_category= Category(m_category);

//    for(auto& v:m_strategies)
//    {
//        programModel->m_data.strategies.push_back(StaticParams::CommonParams::Strategy(v.toInt()));
//    }

//    for(auto& v:m_report)
//    {
//        programModel->m_report.push_back(v.toInt());
//    }



//    for(auto& v:m_dots)
//    {
//        if(fabs(v.toPointF().x())<0.05&&fabs(v.toPointF().y())<0.05)
//        {
//            float x=v.toMap()["x"].toFloat();
//            float y=v.toMap()["y"].toFloat();
//            programModel->m_data.dots.push_back({x,y});
//        }
//        else{
//            programModel->m_data.dots.push_back({float(v.toPointF().x()),float(v.toPointF().y())});
//        }
//    }
//    return programModel->ModelToDB();

//}





//DynamicProgramVM::DynamicProgramVM(const QVariantList & args)
//{
//    if(args.count()==0)
//    {
//        m_dynamicParamsVm=new DynamicParamsVM();
//        return;
//    }
////    qDebug()<<"move programvm cons";
//    int program_id=args[0].toInt();
////    qDebug()<<program_id;
//    qx_query query("select * from program where program_id=:program_id");
//    query.bind(":program_id",program_id);
//    Program_List program_List;
//    QSqlError daoError = qx::dao::execute_query(query, program_List);
//    Program_ptr program_ptr=program_List.first();
////    qDebug()<<program_ptr->m_params;
//    auto programModel=QSharedPointer<DynamicProgramModel>(new DynamicProgramModel(program_ptr));
//    m_id=programModel->m_id;
//    m_name=programModel->m_name;
//    m_type=int(programModel->m_type);
//    m_dynamicParamsVm=new DynamicParamsVM(programModel->m_params);
//    m_category=int(programModel->m_category);

//    for(auto& v:programModel->m_data.dots)
//    {
//        m_dots.append(QPoint(v.x,v.y));
//    }

//    for(auto& v:programModel->m_report)
//    {
//        m_report.append(v);
//    }


//}

//DynamicProgramVM::~DynamicProgramVM()
//{
//    delete m_dynamicParamsVm;
//}

//void DynamicProgramVM::updateProgram()
//{
//    Program_ptr pp=getProgramData();
//    QSqlError error=qx::dao::update(pp);
//}

//void DynamicProgramVM::insertProgram()
//{
//    Program_ptr pp=getProgramData();
//    QSqlError error=qx::dao::insert(pp);
//}

//void DynamicProgramVM::deleteProgram()
//{
//    Program_ptr pp=getProgramData();
//    QSqlError error=qx::dao::delete_by_id(pp);
//}


//Program_ptr DynamicProgramVM::getProgramData()
//{
//    QSharedPointer<DynamicProgramModel> programModel=QSharedPointer<DynamicProgramModel>(new DynamicProgramModel());
//    programModel->m_id=m_id;
//    programModel->m_type=Type(m_type);
//    programModel->m_name=m_name;
////    qDebug()<<m_name;
//    programModel->m_params=m_dynamicParamsVm->getData();
//    programModel->m_category= Category(m_category);



//    for(auto& v:m_dots)
//    {
//        if(fabs(v.toPointF().x())<0.05&&fabs(v.toPointF().y())<0.05)
//        {
//            float x=v.toMap()["x"].toFloat();
//            float y=v.toMap()["y"].toFloat();
//            programModel->m_data.dots.push_back({x,y});
//        }
//        else{
//            programModel->m_data.dots.push_back({float(v.toPointF().x()),float(v.toPointF().y())});
//        }
//    }

//    for(auto& v:m_report)
//    {
//        programModel->m_report.push_back(v.toInt());
//    }


//    return programModel->ModelToDB();
//}


}
