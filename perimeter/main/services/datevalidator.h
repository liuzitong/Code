#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H
#include <QValidator>
namespace Perimeter{
class DateValidator : public QValidator  {
    Q_OBJECT

    Q_PROPERTY( int  minYear  READ  minYear_ts  WRITE setMinYear_ts  NOTIFY minYearChanged )
    Q_PROPERTY( int  maxYear  READ  maxYear_ts  WRITE setMaxYear_ts  NOTIFY maxYearChanged )

public:
    // =================================================================
    // Funtions
    // =================================================================
    Q_INVOKABLE explicit DateValidator( QObject *pa = Q_NULLPTR );
    Q_INVOKABLE virtual ~DateValidator(  ) Q_DECL_OVERRIDE;

    int   minYear_ts( ) const;
    int   maxYear_ts( ) const;

    void  setMinYear_ts( int );
    void  setMaxYear_ts( int );

    virtual void  fixup   ( QString & ) const Q_DECL_OVERRIDE;
    virtual State validate( QString &, int & ) const Q_DECL_OVERRIDE;

    Q_SIGNAL void minYearChanged( );
    Q_SIGNAL void maxYearChanged( );

private:
    void *m_obj;
    Q_DISABLE_COPY( DateValidator )
};
}
#endif // DATEVALIDATOR_H
