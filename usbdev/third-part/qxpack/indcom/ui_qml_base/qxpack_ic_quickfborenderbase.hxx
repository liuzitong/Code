#ifndef QXPACK_IC_QUICKFBORENDERBASE_HXX
#define QXPACK_IC_QUICKFBORENDERBASE_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QQuickFramebufferObject>
#include <QVariant>
#include <QString>
#include <QOpenGLFramebufferObject>
#include <QSharedPointer>

namespace QxPack {

class IcQSGRenderWorker;

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief build a frame buffer object as QuickItem
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcQuickFboRenderBase : public QQuickFramebufferObject {
    Q_OBJECT
public:
    //! create the render object inherit by QOpenGLFunction
    /*!
     * @param ctxt  [in] a user pointer that
     * @param op    [in] the operation
     * @param param [in] parameter, rsvd
     * @return if created object, return RenderWorker pointer (void*) by QVariant
     * @note  op can be "createQSGRenderWorker" -- create a implemented render worker \n
     *        op can be "deleteQSGRenderWorker" -- delete worker pre-created by this factory. \n
     *                                    passed the pointer by 'param' member.
     */
    typedef QVariant (* Factory )( void *ctxt, const QString &op, const QVariant &param );

    //! ctor
    explicit IcQuickFboRenderBase( Factory cr_func, void *cr_ctxt, QQuickItem *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcQuickFboRenderBase( ) Q_DECL_OVERRIDE;

    //! override the create render
    virtual Renderer*  createRenderer() const Q_DECL_OVERRIDE;

protected:
    virtual QOpenGLFramebufferObject *  createFbo_qsg( const QSize & );

private:
    Factory m_cr_func;
    void *m_cr_ctxt;
    void *m_obj;
    Q_DISABLE_COPY( IcQuickFboRenderBase )
    friend class IcQuickFboRenderBase_Render;
};

}

#endif
