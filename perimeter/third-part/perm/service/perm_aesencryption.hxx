#ifndef PERM_AESENCRYPTION_HXX
#define PERM_AESENCRYPTION_HXX

#include "perm/common/perm_def.h"
#include "qxpack/indcom/afw/qxpack_ic_svcbase.hxx"
#include <QByteArray>

namespace FcPerm {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief AES加解密
 */
// ////////////////////////////////////////////////////////////////////////////
class PERM_API AESEncryption : public QxPack::IcSvcBase
{
    Q_OBJECT
public:
    static QByteArray encode(const QByteArray &rawText, const QByteArray &key = QByteArray("abcdefghijkmlnop"));
    static QByteArray decode(const QByteArray &rawText, const QByteArray &key = QByteArray("abcdefghijkmlnop"));
};

}

#endif // PERM_AESENCRYPTION_HXX
