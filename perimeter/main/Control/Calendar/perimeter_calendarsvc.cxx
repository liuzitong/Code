#ifndef PERIMETER_CALENDARSVC_CXX
#define PERIMETER_CALENDARSVC_CXX

#include "perimeter/main/Control/Calendar/perimeter_calendarsvc.hxx"
#include "perimeter/main/Control/Calendar/perimeter_ymdlistmodel.hxx"
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include <QDate>

namespace Perimeter {

#define T_PrivPtr( o )  perimeter_objcast( CalendarSvcPriv*, o )
class PERIMETER_API  CalendarSvcPriv
{
private:
    CalendarSvc   * m_parent;
    YmdListModel  * m_year;
    YmdListModel  * m_month;
    YmdListModel  * m_day;

public :
    explicit CalendarSvcPriv( CalendarSvc * );
    ~CalendarSvcPriv( );

    inline  QObject   * yearObj ( ) { return m_year; }
    inline  QObject   * monthObj( ) { return m_month; }
    inline  QObject   * dayObj  ( ) { return m_day; }
    inline  QString     dateStr ( ) { return QString("%1-%2-%3")
                .arg(m_year->current(), 4, 10, QChar('0'))
                .arg(m_month->current(), 2, 10, QChar('0'))
                .arg(m_day->current(), 2, 10, QChar('0')); }
    inline  QString    currentDate  ( ) { return QDate::currentDate().toString("yyyy-MM-dd"); }
};

CalendarSvcPriv::CalendarSvcPriv(CalendarSvc * pa)
{
    m_parent    = pa;
    m_year      = perimeter_new( YmdListModel, YmdListModel::YEAR );
    m_month     = perimeter_new( YmdListModel, YmdListModel::MONTH );
    m_day       = perimeter_new( YmdListModel, YmdListModel::DAY, m_year, m_month );
}

CalendarSvcPriv::~CalendarSvcPriv()
{
    perimeter_delete( m_year , YmdListModel );
    perimeter_delete( m_month, YmdListModel );
    perimeter_delete( m_day  , YmdListModel );
}

CalendarSvc::CalendarSvc( )
{
    m_obj = perimeter_new( CalendarSvcPriv, this );
}

CalendarSvc::~CalendarSvc()
{
    perimeter_delete( m_obj, CalendarSvcPriv );
}

QObject *CalendarSvc::yearObj() const
{ return T_PrivPtr( m_obj )->yearObj(); }

QObject *CalendarSvc::monthObj() const
{ return T_PrivPtr( m_obj )->monthObj(); }

QObject *CalendarSvc::dayObj() const
{ return T_PrivPtr( m_obj )->dayObj(); }

QString CalendarSvc::dateStr() const
{ return T_PrivPtr( m_obj )->dateStr(); }

QString CalendarSvc::currentDate() const
{ return T_PrivPtr( m_obj )->currentDate(); }

}

#endif // PERIMETER_CALENDARSVC_CXX
