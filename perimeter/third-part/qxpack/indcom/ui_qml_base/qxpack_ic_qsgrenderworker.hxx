#ifndef QXPACK_IC_QSGRENDERWORKER_HXX
#define QXPACK_IC_QSGRENDERWORKER_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QOpenGLFramebufferObject>
#include <QQuickFramebufferObject>
#include <QMetaType>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief the QSG render worker absolute base
 * @note  this class is running in OpenGL thread, not in UI thread.
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQSGRenderWorker {
public:
    //! ctor
    explicit IcQSGRenderWorker( void *rsvd = nullptr );

    //! dtor
    virtual ~IcQSGRenderWorker( );

    /*!
     * @brief initialize this object
     * @return true for OK
     */
    virtual bool initialize( );

    /*!
     * @brief render function
     * @details this render function will be called while QSG render the item
     */
    virtual void render( );

    /*!
     * @brief after called render(), this function is called to determine is require call a \n
     *     update() method.
     * @return default return false
     */
    virtual bool isReqUpdateAfterRender( );

    /*!
     * @brief this is called once before render() called
     * @note used to copy value from QML scene due to render maybe in dedicated thread.
     */
    virtual void  synchronize( QQuickFramebufferObject *item );

private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcQSGRenderWorker )
};


}

#endif
