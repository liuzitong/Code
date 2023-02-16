#ifndef QXPACK_IC_NCSTRING_HXX
#define QXPACK_IC_NCSTRING_HXX

#include <QString>
#include <QByteArray>
#include <QObject>
#include "qxpack/indcom/common/qxpack_ic_def.h"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief non-CaseInsensitive string.
 * @details it provide a non-case insensitive for compare
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcNcString {
public:
    IcNcString( );
    IcNcString( const QString & );
    IcNcString( const QByteArray &ba );
    IcNcString( const char *);
    IcNcString( const IcNcString & );
    virtual ~IcNcString( );
    bool  operator < ( const IcNcString &other ) const;
    void       setString ( const QString & );
    const QString &  strRef( ) const;
private:
    void   *m_obj;
    QString m_str;
};

}

Q_DECLARE_METATYPE( QxPack::IcNcString )

#endif
