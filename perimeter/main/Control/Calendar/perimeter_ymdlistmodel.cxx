#ifndef PERIMETER_YMDLISTMODEL_CXX
#define PERIMETER_YMDLISTMODEL_CXX

#include "perimeter/main/Control/Calendar/perimeter_ymdlistmodel.hxx"
#include "perimeter/base/common/perimeter_memcntr.hxx"
#include <QDate>

#define YEAR_RANGE  100

namespace Perimeter {

#define T_PrivPtr( o )  perimeter_objcast( AbstractYmdListModelPriv*, o )
class PERIMETER_API AbstractYmdListModelPriv
{
public:
    virtual ~AbstractYmdListModelPriv() = default;
    virtual inline int  rowCount( ) = 0;
    virtual QVariant    data( int idx, int role ) = 0;
    virtual int         current( ) = 0;
    virtual void        setCurrent( int ) = 0;
    virtual void        ensureDays( ) = 0;
};
// //////////////////////////////////////////////////////////////////////////
//
//        year date model
//
// //////////////////////////////////////////////////////////////////////////
class PERIMETER_API YearListModelPriv : public AbstractYmdListModelPriv
{
private:
    YmdListModel  * m_parent;
    int             m_currentYear;
    int             m_startYear;

public:
    explicit YearListModelPriv ( YmdListModel * );
    virtual ~YearListModelPriv( ) {}
    inline int  rowCount( ) { return YEAR_RANGE * 1 + 1; }
    QVariant    data( int idx, int role );

    int     current( ) { return m_currentYear; }
    void    setCurrent( int );
    void    ensureDays( ) {}
};

YearListModelPriv::YearListModelPriv(YmdListModel *pa)
{
    m_parent = pa;
    m_currentYear = QDate::currentDate().year();
    m_startYear   = m_currentYear - YEAR_RANGE;
}

QVariant YearListModelPriv::data(int idx, int role)
{
    if ( role == Qt::DisplayRole ) {
        return QVariant( m_startYear + idx );
    } else {
        return QVariant( 0 );
    }
}

void YearListModelPriv::setCurrent(int y)
{
    bool is_need_emit = false;

    if ( y >= m_startYear &&  y < m_startYear + YEAR_RANGE * 1+1 ) {
        if ( y != m_currentYear ) { m_currentYear = y; is_need_emit = true; }
    }
    if ( is_need_emit ) { emit m_parent->currentChanged(); }
}
// //////////////////////////////////////////////////////////////////////////
//
//        Month date model
//
// //////////////////////////////////////////////////////////////////////////
class PERIMETER_API MonthListModelPriv : public AbstractYmdListModelPriv
{
private:
    YmdListModel  * m_parent;
    int             m_currentMonth;

public:
    explicit MonthListModelPriv ( YmdListModel * );
    virtual ~MonthListModelPriv( ) {}
    inline int  rowCount( ) { return 12; }
    QVariant    data( int idx, int role );

    int     current( ) { return m_currentMonth; }
    void    setCurrent( int );
    void    ensureDays( ){}
};

MonthListModelPriv::MonthListModelPriv(YmdListModel *pa)
{
    m_parent = pa;
    m_currentMonth = QDate::currentDate().month();
}

QVariant MonthListModelPriv::data(int idx, int role)
{
    if ( role == Qt::DisplayRole ) {
        return QVariant( idx + 1 );
    } else {
        return QVariant( 0 );
    }
}

void MonthListModelPriv::setCurrent(int m)
{
    bool is_need_emit = false;
    if ( m >= 1 && m <= 12 ) {
        if ( m_currentMonth != m ) { m_currentMonth = m; is_need_emit = true; }
    }
    if ( is_need_emit ) { emit m_parent->currentChanged(); }
}
// //////////////////////////////////////////////////////////////////////////
//
//        day date model
//
// //////////////////////////////////////////////////////////////////////////
class PERIMETER_API DayListModelPriv : public AbstractYmdListModelPriv
{
private:
    YmdListModel  * m_parent;
    YmdListModel  * m_year;
    YmdListModel  * m_month;
    int             m_currentDay;
    int             m_dayCount;

public:
    explicit DayListModelPriv ( YmdListModel *, YmdListModel *, YmdListModel * );
    virtual ~DayListModelPriv( ) {}
    inline int  rowCount( ) { return m_dayCount; }
    QVariant    data( int idx, int role );

    int     current( ) { return m_currentDay; }
    void    setCurrent( int );
    void    ensureDays( );
};

DayListModelPriv::DayListModelPriv(YmdListModel * pa, YmdListModel * ym, YmdListModel *mm)
{
    m_parent = pa; m_year = ym; m_month = mm;

    int currentYear  = ym->current();
    int currentMonth = mm->current();

    QDate dt( currentYear, currentMonth, 1 );
    QDate cdt = QDate::currentDate();

    m_dayCount = dt.daysInMonth();
    if ( cdt.day() <= m_dayCount ) {
        m_currentDay = cdt.day(); // 1 to 31
    } else {
        m_currentDay = 1;
    }

    QObject::connect( m_year, &YmdListModel::currentChanged, m_parent, &YmdListModel::ensureDays );
    QObject::connect( m_month, &YmdListModel::currentChanged, m_parent, &YmdListModel::ensureDays );
}

QVariant DayListModelPriv::data(int idx, int role)
{
    if ( role == Qt::DisplayRole ) {
        return QVariant( idx + 1 );
    } else {
        return QVariant( 0 );
    }
}

void DayListModelPriv::setCurrent(int d)
{
    bool is_need_emit = false;
    if ( d >= 1 && d <= m_dayCount ) {
       if ( m_currentDay != d ) { m_currentDay = d; is_need_emit = true; }
    }
    if ( is_need_emit ) { emit m_parent->currentChanged(); }
}

static const int days_in_month[ ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
void DayListModelPriv::ensureDays()
{
    int days = days_in_month[ m_month->current() - 1 ];

    if ( m_month->current() == 2  &&  QDate::isLeapYear( m_year->current() )) {
        ++ days;
    }

    if ( m_dayCount < days  ) {
        m_parent->beginInsertRows( QModelIndex(), m_dayCount, days - 1 );
        m_dayCount = days;
        m_parent->endInsertRows();

    } else if ( m_dayCount > days ) {
        m_parent->beginRemoveRows( QModelIndex(), days, m_dayCount - 1 );
        m_dayCount = days;
        m_parent->endRemoveRows();
        if ( m_currentDay > days ) { m_currentDay = days; emit m_parent->current(); }

    } else { }
}

YmdListModel::YmdListModel(YmdListModel::DateType type)
{
    if ( type == YmdListModel::YEAR )
        m_obj = perimeter_new( YearListModelPriv, this );
    if ( type == YmdListModel::MONTH )
        m_obj = perimeter_new( MonthListModelPriv, this );
}

YmdListModel::YmdListModel(YmdListModel::DateType type, YmdListModel *ym, YmdListModel *mm)
{
    if ( type == YmdListModel::DAY )
        m_obj = perimeter_new( DayListModelPriv, this, ym, mm );
}

YmdListModel::~YmdListModel()
{
    perimeter_delete( m_obj, AbstractYmdListModelPriv );
}

int YmdListModel::current()
{ return T_PrivPtr( m_obj )->current(); }

void YmdListModel::setCurrent(int v)
{ T_PrivPtr( m_obj )->setCurrent( v ); }

// ============================================================================
// return the total count of records
// ============================================================================
int  YmdListModel :: rowCount(const QModelIndex &) const
{ return T_PrivPtr( m_obj )->rowCount(); }

// ============================================================================
// return the role flags
// ============================================================================
Qt::ItemFlags  YmdListModel :: flags ( const QModelIndex &idx ) const
{  return ( idx.isValid() ? QAbstractItemModel::flags( idx ) | Qt::ItemIsUserCheckable : Qt::NoItemFlags );}

void YmdListModel::ensureDays()
{ T_PrivPtr( m_obj )->ensureDays( ); }

// ============================================================================
// return the role names used by model
// ============================================================================
QHash<int,QByteArray>  YmdListModel :: roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert( Qt::DisplayRole,  "currentValue" );
    return roles;
}

// ============================================================================
// return the data by index and role
// ============================================================================
QVariant   YmdListModel :: data ( const QModelIndex &idx, int role ) const
{  return T_PrivPtr( m_obj )->data( idx.row(), role );  }

// ============================================================================
// set the data, now allowed.
// ============================================================================
bool       YmdListModel :: setData ( const QModelIndex &, const QVariant &, int )
{ return false; }

}

#endif // PERIMETERE_YMDLISTMODEL_CXX
