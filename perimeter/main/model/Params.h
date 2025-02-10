#ifndef PARAMS_H
#define PARAMS_H

#define OPTION int
#define ON_OFF bool
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <utility>
#include "Point.h"
#include <QString>
#include <sstream>
#include <exception>
#include <iostream>
#include <QObject>


enum class CursorColor{white,red,blue};
enum class CursorSize{I,II,III,IV,V};
enum class BackGroundColor{white,yellow};
enum class FixationTarget{centerPoint,smallDiamond,bigDiamond,bottomPoint};
enum class FixationMonitor{onlyAlarm,alarmAndPause};
enum class Type { ThreshHold=0, Screening,Dynamic  };
enum class Category { ThreshHold=0, Screening,Special,Dynamic,Custom  };


struct StaticParams/*:public QObject*/
{
//    Q_OBJECT
//    Q_PROPERTY(QObject commonParams READ getCommonParams WRITE setCommonParams)
//    Q_PROPERTY(QObject patientId READ getPatientID WRITE setPatientID)
    struct CommonParams/*:public QObject*/
    {
        /**
         * @brief The Strategy enum
         * Strategy 决定的是每个点值的测试方式
         * fullThreshold,fastThreshold,smartInteractive,fastInterative 应用于阈值 oneStage,twoStages,quantifyDefects,singleStimulation 应用于筛选
         * fullThreshold,fastThreshold 是根据年龄查表选定不同的初始DB.测试顺序完全随机.
         * smartInteractive,fastInterative是根据周围的DB决定下一个初始DB,最初4个是根据ageRelated查表得来的,4个点坐标固定.每次随机选择这4个点中的一个点,测离这个点最近点的值.
         * oneStage,twoStages,quantifyDefects,singleStimulation 测试顺序完全随机.
         *                                         ->46(×) =>db=44
         * fullThreshold 全阈值 40(√)->44(√)->48(×)
         *                                         ->46(√) =>db=46
         *                                           ->43(×) =>db=41
         * fastThreshold 快速阈值 34(√)->40(√)->46(×)
         *                                           ->43(√) =>db=44
         *                                                ->46(×) =>db=45或者44,参考相邻点,最好比低角度的小,比高角度的大,策略模式仅可使用ThresholdRelated
         * smartInteractive 智能交互式  40(√)->44(√)->48(×)
         *                                                ->46(√) =>db=46或者47,最初的点,比如中心点还有参考的4个点,要多测一次确定最后一个
         * fastInterative 快速智能交互式  40(√)->43(√)->46(×) => db=44
         * oneStage 一区法 是根据年龄表查到标准DB,在此基础上-4,应答就是看到(□),没应答就是未看到(■)
         * twoStages 二区法 是根据年龄表查到标准DB,在此基础上-4,应答就是看到(□),没应答之后会亮一个0DB的点,应答就是弱视(×),未应答就是未看到(■).
         * quantifyDefects 量化缺损 是根据年龄表查到标准DB,在此基础上-4,应答就是看到(□),没应答就按继续按fastInterative的方式测出具体值.标准值与具体值的差值为缺损值.
         * singleStimulation 单刺激 刺激所有DB根据设置,应答就是看到(□),没应答就是未看到(■).
         *
         * 筛选 数据处理方式0 未看到,1弱视,2看到,4以上的是量化缺损值.
         */
        enum class Strategy{fullThreshold,fastThreshold,smartInteractive,fastInterative,oneStage,twoStages,quantifyDefects,singleStimulation};
//        Q_OBJECT

        //不需要了
        /**
         * @brief The StrategyMode enum
         * Strategy 决定的是点的起始值和点的测试顺序
         * ageRelated,ThresholdRelated 用于阈值 fastInterative,oneStage,twoStages,quantifyDefects,singleStimulation 用于筛选
         * ageRelated 是根据年龄查表选定不同的初始DB.测试顺序完全随机.
         * ThresholdRelated是根据周围的DB决定下一个初始DB,最初4个是根据ageRelated查表得来的,4个点坐标固定.每次随机选择这4个点中的一个点,测离这个点最近点的值.
         */
        enum class StrategyMode{ageRelated,ThresholdRelated};

        int                         Range[2];
        Strategy                    strategy;
        StrategyMode                strategyMode;                   //obsolete
        CursorColor                 cursorColor;                    //done
        CursorSize                  cursorSize;                     //done
        BackGroundColor             backGroundColor;                //done

        /**
         * @brief blueYellowTest
         * 打开此开光,刺激设置为蓝色,5号大小,背景光为黄色.相关UI联动显示,并置为只读.
         */
        ON_OFF                      blueYellowTest;                     //done
        /**
         * @brief responseAutoAdapt
         * 自适应时间
         * 开起此项算出应答时间平均值,加上延迟时间,取代intervalTime
         */
        ON_OFF                      responseAutoAdapt;                  //check proscess
        /**
         * @brief responseDelayTime
         * 相应延迟时间
         */
        int                         responseDelayTime;
        /**
         * @brief centerDotCheck
         * 中心点检测,打开此开关,开始的时候测试版中心点,如果中心点是固视点,那么就测试菱形中心点.
         */
        ON_OFF                      centerDotCheck;                         //done
        /**
         * @brief shortTermFluctuation
         * 短周期波动测试, 即是一个位置测试两次(测试方式按照策),得到两个结果.周期长度按照固定参数设置(目前默认是10);
         * 测试方式是测完周期长度的数目的点,再随机选中一个点再次测试.第二次结果以括号的形式写在旁边(打印,实时均要求此项).
         * 目前考虑的处理方法是,设置结果设置为两倍长度,第一倍是普通结果,第二倍是短周期波动结果.-1表示没有测试.
         */
        ON_OFF                      shortTermFluctuation;                  //done
        /**
         * @brief fixationTarget
         * 有中心点,大菱形,小菱形,底点,这几种固视.Y坐标分别,偏移0,-8,-8(大小菱形同心),-12度.
         */
        FixationTarget              fixationTarget;                 //done
        FixationMonitor             fixationMonitor;                //即是eye move alarm mode         //todo 在on take frame 处理
        /**
         * @brief blindDotTest
         * 盲点测试,经过固视率丢失周期次数, 之后在盲点测试一次,应答视为固视丢失.
         */
        ON_OFF                      blindDotTest;                  //donte

        template<class Archive>
        void serialize(Archive& archive, const unsigned int version)
        {
            archive & BOOST_SERIALIZATION_NVP(Range);
            archive & BOOST_SERIALIZATION_NVP(strategy);
            archive & BOOST_SERIALIZATION_NVP(strategyMode);
            archive & BOOST_SERIALIZATION_NVP(cursorColor);
            archive & BOOST_SERIALIZATION_NVP(cursorSize);
            archive & BOOST_SERIALIZATION_NVP(backGroundColor);
            archive & BOOST_SERIALIZATION_NVP(blueYellowTest);
            archive & BOOST_SERIALIZATION_NVP(responseAutoAdapt);
            archive & BOOST_SERIALIZATION_NVP(responseDelayTime);
            archive & BOOST_SERIALIZATION_NVP(centerDotCheck);
            archive & BOOST_SERIALIZATION_NVP(shortTermFluctuation);
            archive & BOOST_SERIALIZATION_NVP(fixationTarget);
            archive & BOOST_SERIALIZATION_NVP(fixationMonitor);
            archive & BOOST_SERIALIZATION_NVP(blindDotTest);
        }

    };

    struct FixedParams/*:public QObject*/
    {
//        Q_OBJECT
        /**
         * @brief stimulationTime
         * 刺激时间
         */
        int stimulationTime;
        /**
         * @brief intervalTime
         * 间隔时间
         * 等关闭到下一次灯亮的时间
         */
        int intervalTime;

        /**
         * @brief falsePositiveCycle
         * 假阳性：在测试过程中，投射器转动到一定位置,但是快门关闭，如果不响应就正常，如果响应就记录一次假阳性。
         * 此为周期,即是经过固定个点,搞一次
         * 周期为亮点次数
         */
        int falsePositiveCycle;  //参数作废，改为从灯开始亮的时候到200ms内应答的为假阳性，此次响应结果作废。
        /**
         * @brief falseNegativeCycle
         * 假阴性：在曾经响应过的位置，再减少几个DB的亮度(即是更亮)再次测试，如响应就正常，不响应则记录一次假阴性。
         * 此为周期,即时经过固定个点,在上个周期数个点之中随机调一个做此项测试.
         * 周期为亮点次数
         */
        int falseNegativeCycle;  //参数作废，改为离心度相同或更小的点和前面的目标点进行比较，目标点响应了，而后面的点在比目标点还低4db以上时仍未响应，记录一次假阴性。
        /**
         * @brief fixationViewLossCycle
         * 固视丢失周期,属于盲点测试打开的情况下的,测试这个亮点次数后,在盲点刺激,应答就属于一次固视丢失
         * 周期为亮点次数
         */
        int fixationViewLossCycle;
        /**
         * @brief singleStimulationDB
         * 单刺激亮度,属于常用参数种的单刺激策略.
         */
        int singleStimulationDB;
        /**
         * @brief blindDotStimulationDB
         * 盲点为位置刺激亮度,盲点测试用
         */
        int blindDotStimulationDB;
        int shortTermFluctuationCount;
        int leastWaitingTime;


        template<class Archive>
        void serialize(Archive& archive, const unsigned int version)
        {
            archive & BOOST_SERIALIZATION_NVP(stimulationTime);
            archive & BOOST_SERIALIZATION_NVP(intervalTime);
            archive & BOOST_SERIALIZATION_NVP(falsePositiveCycle);
            archive & BOOST_SERIALIZATION_NVP(falseNegativeCycle);
            archive & BOOST_SERIALIZATION_NVP(fixationViewLossCycle);
            archive & BOOST_SERIALIZATION_NVP(singleStimulationDB);
            archive & BOOST_SERIALIZATION_NVP(blindDotStimulationDB);
            archive & BOOST_SERIALIZATION_NVP(shortTermFluctuationCount);
            archive & BOOST_SERIALIZATION_NVP(leastWaitingTime);
        }

    };


    CommonParams commonParams;
    FixedParams  fixedParams;

//    CommonParams& getCommonParams(){return commonParams;}

    template<class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & BOOST_SERIALIZATION_NVP(commonParams);
        archive & BOOST_SERIALIZATION_NVP(fixedParams);
    }
};

struct DynamicParams
{
    /**
     * @brief The Strategy enum
     *
     */
    enum class Strategy{standard,blindArea,darkArea,straightLine};
    enum class DynamicMethod{_4Lines,_6Lines,_8Lines};
    enum class DynamicDistance{_5,_10,_15};
    int                         Range[2];
    Strategy                    strategy;
    CursorColor                 cursorColor;
    CursorSize                  cursorSize;
    BackGroundColor             backGroundColor;

//    FixationTarget       fixationTarget;
    int                         speed;
    int                         brightness;
    DynamicMethod               dynamicMethod;
    DynamicDistance             dynamicDistance;
    FixationMonitor             fixationMonitor;

    template<class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & BOOST_SERIALIZATION_NVP(Range);
        archive & BOOST_SERIALIZATION_NVP(strategy);
        archive & BOOST_SERIALIZATION_NVP(cursorColor);
        archive & BOOST_SERIALIZATION_NVP(cursorSize);
        archive & BOOST_SERIALIZATION_NVP(backGroundColor);
//        archive & BOOST_SERIALIZATION_NVP(fixationTarget);
        archive & BOOST_SERIALIZATION_NVP(speed);
        archive & BOOST_SERIALIZATION_NVP(brightness);
        archive & BOOST_SERIALIZATION_NVP(dynamicMethod);
        archive & BOOST_SERIALIZATION_NVP(dynamicDistance);
        archive & BOOST_SERIALIZATION_NVP(fixationMonitor);
    }

//    QString ToQString()
//    {
//        std::stringstream ss;
//        boost::archive::xml_oarchive oa(ss);
//        oa& BOOST_SERIALIZATION_NVP(*this);
//        return ss.str().c_str();
//    }
};

//template <Type T>
//struct ParamTraits
//{
//    typedef StaticParams params;
//};


//template <>
//struct ParamTraits<Type::Dynamic>
//{
//    typedef DynamicParams params;
//};



#endif // PARAMS_H
