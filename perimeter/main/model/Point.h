#ifndef POINT_H
#define POINT_H
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <QPointF>

struct Point                         //标准动态程序的测试点的室 x表示长度,y表示角度
{
    float x;                    //radius
    float y;                    //angle

    QPointF toQPointF()
    {
        return QPointF{x,y};
    }
    template<class Archive>
    void serialize(Archive& archive, const unsigned int version)
    {
        archive & BOOST_SERIALIZATION_NVP(x);
        archive & BOOST_SERIALIZATION_NVP(y);
    }

    void operator=(QPointF point)
    {
        x=point.x();
        y=point.y();
    }
};





#endif // COMMONDEF_H

