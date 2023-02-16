#ifndef PERIMETER_CALENDARSVC_HXX
#define PERIMETER_CALENDARSVC_HXX

#include "perimeter/base/common/perimeter_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"

namespace Perimeter {

class PERIMETER_API CalendarSvc : public QxPack::IcSvcBase
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit CalendarSvc( );
    virtual ~CalendarSvc( ) Q_DECL_OVERRIDE;

    QObject   * yearObj ( ) const;
    QObject   * monthObj( ) const;
    QObject   * dayObj  ( ) const;
    QString     dateStr ( ) const;
    QString     currentDate() const;

private:
    void *m_obj;
    Q_DISABLE_COPY( CalendarSvc )
};

}

#endif // PERIMETER_CALENDARSVC_HXX
