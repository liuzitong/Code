#ifndef QXPACK_IC_SVCBASE_HXX
#define QXPACK_IC_SVCBASE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QGenericArgument>
#include <QGenericReturnArgument>

namespace QxPack {

// ////////////////////////////////////////////////////
/*!
 * @brief service base class
 */
// ////////////////////////////////////////////////////
class QXPACK_IC_API  IcSvcBase : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcSvcBase )
public:
    //! ctor
    explicit IcSvcBase ( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcSvcBase( ) Q_DECL_OVERRIDE;

    //! call the method of the service
    /*!
     * @details this routine will call child class's Q_INVOKABLE/Q_SLOT method.
     */
    virtual bool  call(
        const char * method_name,
        QGenericReturnArgument ret,
        QGenericArgument val0 = QGenericArgument( Q_NULLPTR ),
        QGenericArgument val1 = QGenericArgument(),
        QGenericArgument val2 = QGenericArgument(),
        QGenericArgument val3 = QGenericArgument(),
        QGenericArgument val4 = QGenericArgument(),
        QGenericArgument val5 = QGenericArgument(),
        QGenericArgument val6 = QGenericArgument(),
        QGenericArgument val7 = QGenericArgument()
    );

    //! call the method of the service without return argument
    virtual bool  call(
        const char * method_name,
        QGenericArgument val0 = QGenericArgument( Q_NULLPTR ),
        QGenericArgument val1 = QGenericArgument(),
        QGenericArgument val2 = QGenericArgument(),
        QGenericArgument val3 = QGenericArgument(),
        QGenericArgument val4 = QGenericArgument(),
        QGenericArgument val5 = QGenericArgument(),
        QGenericArgument val6 = QGenericArgument(),
        QGenericArgument val7 = QGenericArgument()
    );


private:
    void *m_obj;
};


}

#endif
