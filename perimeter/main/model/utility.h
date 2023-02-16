#ifndef UTILITY_H
#define UTILITY_H

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <QString>
#include <sstream>
#include <perimeter/main/model/Params.h>
//#include <perimeter/main/model/utility.cpp>



class Utility
{
public:
    Utility()=default;
    template<typename T>
    QString static entityToQString(T t)
    {
        std::stringstream ss;                              //必须括号主动调用析构函数,不然写入不全
        {
            boost::archive::xml_oarchive oa(ss);
            oa<< BOOST_SERIALIZATION_NVP(t);
        }
//        std::cout<<ss.str()<<std::endl;
        return QString(ss.str().c_str());
//        return ss.str().c_str();
    }
    template<typename T>
    T static QStringToEntity(QString str)
    {
        T t;
        std::stringstream ss;
        ss<<str.toStdString();
        boost::archive::xml_iarchive ia(ss);
        ia>> BOOST_SERIALIZATION_NVP(t);
        return t;
    }
};


#endif // UTILITY_H
