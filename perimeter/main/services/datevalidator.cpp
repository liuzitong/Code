#ifndef ALBIO_DATEVALIDATOR_CXX
#define ALBIO_DATEVALIDATOR_CXX

#include "datevalidator.h"
#include <QDate>
#include "perimeter/base/common/perimeter_memcntr.hxx"
#define YEAR_RANGE  100

namespace Perimeter {

#define T_PrivPtr( o )  perimeter_objcast( DateValidatorPriv*, o )
class  DateValidatorPriv {
private:
    DateValidator  *m_parent;
    int  m_min_year, m_max_year;

public :
    DateValidatorPriv ( DateValidator *pa );
    ~DateValidatorPriv (  ) { }

    inline int minYear( ) { return m_min_year; }
    inline int maxYear( ) { return m_max_year; }
    void  setMinYear( int y );
    void  setMaxYear( int y );
    void  fixup( QString & );
    QValidator::State  validate( QString &,int& );
};
// =====================================================================
// CTOR
// =====================================================================
DateValidatorPriv :: DateValidatorPriv ( DateValidator *pa )
{
    m_parent = pa;
    QDate dt = QDate::currentDate( );
    m_min_year = dt.year( ) - YEAR_RANGE;
    m_max_year = dt.year( ) + YEAR_RANGE;
}

// ======================================================================
// set the min year
// ======================================================================
void DateValidatorPriv :: setMinYear( int y )
{
    if ( m_min_year != y ) {
        m_min_year = y;
        emit m_parent->minYearChanged( );
    }
}

// =====================================================================
// set the max year
// =====================================================================
void DateValidatorPriv :: setMaxYear ( int y )
{
    if ( m_max_year != y ) {
        m_max_year = y;
        emit m_parent->maxYearChanged( );
    }
}

// =====================================================================
// fixup the string
// =====================================================================
void  DateValidatorPriv :: fixup ( QString &str )
{
    int pos = 1; this->validate( str, pos );
}

// ======================================================================
// validate the string
// ======================================================================
QValidator::State  DateValidatorPriv :: validate( QString &input, int &pos )
{
    // nw: 2018/05/31 fixed the insert pos if at 4, 7.
    if ( input.size() >= 10 ) { // format is 2000-11-11
        if ( pos == 4 ) { pos = 5; }
        if ( pos == 7 ) { pos = 8; }
    }

    // in QT5.x , pos is start with 1
    if ( pos <= 0  ) {
        input = QDate::currentDate().toString( QStringLiteral("yyyy-MM-dd"));
        return DateValidator::Acceptable;
    }
    if ( QDate::fromString( input ).isValid() ) {
        return DateValidator::Acceptable;
    }

    // check the section part.
    QString  sec_y = input.section('-',0,0).trimmed().left(4);
    QString  sec_m = input.section('-',1,1).trimmed().left(2);
    QString  sec_d = input.section('-',2,2).trimmed().left(2);

    if ( sec_y.isEmpty() && sec_m.isEmpty() && sec_d.isEmpty()) {
        input = QDate::currentDate().toString( QStringLiteral("yyyy-MM-dd"));
        return DateValidator::Acceptable;
    }

    // check every field
    bool is_year_ok = false; int year = sec_y.toInt( & is_year_ok, 10 );
    bool is_mon_ok  = false; int mon  = sec_m.toInt( & is_mon_ok,  10 );
    bool is_day_ok  = false; int day  = sec_d.toInt( & is_day_ok,  10 );

    if ( is_year_ok ) {
        if ( year < m_min_year ) { year = m_min_year; }
        if ( year > m_max_year ) { year = m_max_year; }
    } else {
        year = QDate::currentDate().year();
    }

    if ( is_mon_ok ) {
        if ( mon < 1  ) { mon = 1;  }
        if ( mon > 12 ) { mon = 12; }
    } else {
        mon = 1;
    }

    if ( is_day_ok ) {
        if ( day <= 0 ) {
            day = 1;
        } else {
            QDate dt = QDate( year, mon, 1 );
            int sum_day = dt.daysInMonth();
            if ( day >= sum_day ) {
                day = sum_day;
            }
        }
    } else {
        day = 1;
    }

    input = QString("%1-%2-%3").arg( year, 4, 10, QChar('0')).arg( mon, 2, 10, QChar('0')).arg( day, 2, 10, QChar('0'));
    return DateValidator::Intermediate;
}

// /////////////////////////////////////////////////////////////////////
//
//    DateValidator
//
// /////////////////////////////////////////////////////////////////////
// =====================================================================
//!  CTOR
// =====================================================================
DateValidator :: DateValidator ( QObject *pa ) : QValidator( pa )
{
    m_obj = perimeter_new( DateValidatorPriv, this );
}

// =====================================================================
//!  DTOR
// =====================================================================
DateValidator :: ~DateValidator (  )
{
    perimeter_delete( m_obj, DateValidatorPriv );
}


// =====================================================================
//! return minimize year
// =====================================================================
int  DateValidator :: minYear_ts( ) const { return T_PrivPtr( m_obj )->minYear(); }

// =====================================================================
//! return maximum year
// =====================================================================
int  DateValidator :: maxYear_ts( ) const { return T_PrivPtr( m_obj )->maxYear( ); }

// =====================================================================
//! set the minimum year
// =====================================================================
void DateValidator :: setMinYear_ts( int y ) { T_PrivPtr( m_obj )->setMinYear( y ); }

// =====================================================================
//! set the maximum year
// =====================================================================
void  DateValidator :: setMaxYear_ts( int y ) { T_PrivPtr( m_obj )->setMaxYear( y ); }

// =====================================================================
//! fixup the input string
// =====================================================================
void  DateValidator :: fixup   ( QString &input ) const { T_PrivPtr( m_obj )->fixup( input ); }

// =====================================================================
//! validate the input string
// =====================================================================
DateValidator::State  DateValidator :: validate( QString &input, int &pos ) const
{   return T_PrivPtr( m_obj )->validate( input, pos ); }

}

#endif // ALBIO_DATEVALIDATOR_CXX
