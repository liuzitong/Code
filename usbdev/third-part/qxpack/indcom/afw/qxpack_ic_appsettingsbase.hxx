#ifndef QXPACK_IC_APPSETTINGSBASE_HXX
#define QXPACK_IC_APPSETTINGSBASE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QJsonObject>
#include <QVariantList>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief the application settings manager
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcAppSettingsBase : public QObject {
    Q_OBJECT
public:
    //! ctro
    explicit IcAppSettingsBase( const QString &setting_file_path, QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcAppSettingsBase() Q_DECL_OVERRIDE;

    //! @return the version string of settings
    Q_INVOKABLE QString      version ( ) const;

    //! @return the specified module configration json object
    Q_INVOKABLE QJsonObject  cfgObject( const QString &mod_name );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcAppSettingsBase )
};


}

#endif
