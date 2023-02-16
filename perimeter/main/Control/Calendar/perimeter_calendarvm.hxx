#ifndef PERIMETER_CALENDARVM_HXX
#define PERIMETER_CALENDARVM_HXX

#include "perimeter/base/common/perimeter_def.h"
#include "qxpack/indcom/afw/qxpack_ic_viewmodelbase.hxx"


namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 界面容器页，用于管理顶层页面
 */
// ////////////////////////////////////////////////////////////////////////////
class PERIMETER_API CalendarVm : public QxPack::IcViewModelBase
{
    Q_OBJECT
    Q_PROPERTY(QObject * yearListModel  READ yearObj  CONSTANT)
    Q_PROPERTY(QObject * monthListModel READ monthObj CONSTANT)
    Q_PROPERTY(QObject * dayListModel   READ dayObj   CONSTANT)
    Q_PROPERTY(QString   dateStr        READ dateStr  CONSTANT)
    Q_PROPERTY(QString   currentDate    READ currentDate  CONSTANT)
public:
    Q_INVOKABLE explicit CalendarVm( const QVariantList & );
    Q_INVOKABLE virtual ~CalendarVm( ) Q_DECL_OVERRIDE;

    QObject   * yearObj ( ) const;
    QObject   * monthObj( ) const;
    QObject   * dayObj  ( ) const;
    QString     dateStr ( ) const;
    QString     currentDate() const;
private:
    void *m_obj;
    Q_DISABLE_COPY( CalendarVm )
};

}

#endif // PERIMETER_CALENDARVM_HXX
