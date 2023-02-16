// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_objmgr
  @author  night wing
  @date    2019/1, 2019/04, 2015.05
  @version 0.2.0.0
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_OBJMGR_HXX
#define QXPACK_IC_OBJMGR_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QThread>
#include <QString>
#include <QVariantList>
#include <QMetaObject>

#ifdef QXPACK_IC_DETECTED_QML
#include <QQmlEngine>
#  define QQMLENGINE_SetObjOwnership  reinterpret_cast<SetObjOwnershipFunc>( &QQmlEngine::setObjectOwnership )
#else
#  define QQMLENGINE_SetObjOwnership  Q_NULLPTR
#endif

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief object manager
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcObjMgr : public QObject {
    Q_OBJECT
public:
    //! a callback function that do call QMLEngine::setObjectOwnership
    typedef void (* SetObjOwnershipFunc )( QObject*, qintptr );

    //! ctor
    //! @param cb_func [in] the object ownership callback function
    //! @param pa      [opt_in] object parent pointer
    explicit IcObjMgr (
        SetObjOwnershipFunc cb_func = QQMLENGINE_SetObjOwnership, QObject *pa = Q_NULLPTR
    );

    //! dtor
    virtual ~IcObjMgr() Q_DECL_OVERRIDE;

    //! a template function that wrapped the registerType(name,tgt,metaobject )
    //! @param  name [in]  a unique name string in object manager
    //! @param  tgt  [in]  target thread, Q_NULLPTR means create object in caller thread
    //! @return true means registered.
    //! @note  the register class must be has CTOR like below:
    //!     Q_INVOKABLE  ClassCtor( const QVaraintList & );
    template < typename T >
    inline bool registerType( const QString & name, QThread *tgt )
    {
        return this->registerType( name, tgt, T::staticMetaObject );
    }

    //! meta function about attach a object
    //! @param name       [in]  registered name about this class
    //! @param req_global [in]  true means attach to a global object
    //! @param va         [opt_in]  optional, ctor parameters
    //! @return a pointer to object if created or attached
    Q_INVOKABLE QObject*  attachObj( const QString &name, bool req_global, const QVariantList &va = QVariantList());

    //! meta function about detach from a object
    //! @param name       [in]  registered name about this class
    //! @param obj        [in]  the object pointer
    //! @return true means detached
    Q_INVOKABLE bool      detachObj( const QString &name, QObject *obj );

protected:
    bool  registerType ( const QString &name, QThread *tgt, const QMetaObject &mo );
private:
    void *m_obj;
    Q_DISABLE_COPY( IcObjMgr )
};

}

#endif
