// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_vardata
  @author  night wing
  @date    2018/09, 2019/05
  @brief   wrap of the QVariant
  @version 0.2.0.0
*/
// /////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_VARDATA_HXX
#define QXPACK_IC_VARDATA_HXX

#include <qxpack/indcom/common/qxpack_ic_def.h>
#include <QObject>
#include <QVariant>
#include <QString>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief this class is a wrap of the QVariant
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcVarData QXPACK_IC_FINAL {
public:
    //! empty ctro
    IcVarData ( );

    //! create object by variant
    IcVarData ( const QVariant &var );

    //! create object by another
    IcVarData ( const IcVarData & );

    //! assign copy
    IcVarData &  operator = ( const IcVarData & );

    //! dtor
    //virtual ~IcVarData( ); // since 0.4.8, changed to non-virtual
    ~IcVarData();

    //! check if an null object
    bool     isNull ( ) const;

    //! the serial id setup by user
    qintptr   id ( ) const;

    //! the tag setup by user
    qintptr   tag( ) const;

    //! the name of this vardata
    QString   name( ) const;

    //! the variant data setup by user
    QVariant  variant( ) const;

    //! set id
    void  setId( qintptr );

    //! set tag
    void  setTag( qintptr );

    //! set the name
    void  setName( const QString & );

    //! set variant
    void  setVariant( const QVariant & );

private:
    void *m_obj;
};

}

Q_DECLARE_METATYPE( QxPack::IcVarData )

#endif
