#ifndef PERIMETER_CALENDARVM_CXX
#define PERIMETER_CALENDARVM_CXX

#include "perimeter/main/Control/Calendar/perimeter_calendarvm.hxx"
#include "perimeter/main/Control/Calendar/perimeter_calendarsvc.hxx"
#include "perimeter/base/common/perimeter_memcntr.hxx"

namespace Perimeter {

#define T_PrivPtr( o )  perimeter_objcast( CalendarVmPriv*, o )
class PERIMETER_API CalendarVmPriv
{
private:
    CalendarVm    * m_parent;
    CalendarSvc     m_calSvc;

public:
    explicit CalendarVmPriv ( CalendarVm *pa );
    ~CalendarVmPriv( );

    inline  QObject   * yearObj ( ) { return m_calSvc.yearObj(); }
    inline  QObject   * monthObj( ) { return m_calSvc.monthObj(); }
    inline  QObject   * dayObj  ( ) { return m_calSvc.dayObj(); }
    inline  QString     dateStr ( ) { return m_calSvc.dateStr(); }
    inline  QString     currentDate ( ) { return m_calSvc.currentDate(); }
};

CalendarVmPriv::CalendarVmPriv(CalendarVm *pa)
{
    m_parent = pa;
}

CalendarVmPriv::~CalendarVmPriv()
{
}

CalendarVm::CalendarVm(const QVariantList &)
{
    m_obj = perimeter_new( CalendarVmPriv, this );
//    qDebug()<<"calcendarVm says hi";
}

CalendarVm::~CalendarVm()
{
    perimeter_delete( m_obj, CalendarVmPriv );
}

QObject *CalendarVm::yearObj() const
{ return T_PrivPtr( m_obj )->yearObj(); }

QObject *CalendarVm::monthObj() const
{ return T_PrivPtr( m_obj )->monthObj(); }

QObject *CalendarVm::dayObj() const
{ return T_PrivPtr( m_obj )->dayObj(); }

QString CalendarVm::dateStr() const
{ return T_PrivPtr( m_obj )->dateStr(); }

QString CalendarVm::currentDate() const
{ return T_PrivPtr( m_obj )->currentDate(); }

}

#endif // PERIMETER_CALENDARVM_CXX
