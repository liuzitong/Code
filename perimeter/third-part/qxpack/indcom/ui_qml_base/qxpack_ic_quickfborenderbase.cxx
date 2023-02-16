#ifndef QXPACK_IC_QUICKFBORENDERBASE_CXX
#define QXPACK_IC_QUICKFBORENDERBASE_CXX

#include "qxpack_ic_quickfborenderbase.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include <QQuickWindow>
#include "qxpack/indcom/ui_qml_base/qxpack_ic_qsgrenderworker.hxx"

#if defined( QXPACK_IC_QT6 )
#include <QQuickOpenGLUtils>
#endif

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// render object
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN IcQuickFboRenderBase_Render : public QQuickFramebufferObject::Renderer {
private:
    IcQSGRenderWorker *m_worker;
    IcQuickFboRenderBase::Factory m_fact; void *m_fact_ctxt;
    IcQuickFboRenderBase   *m_item;
public :
    IcQuickFboRenderBase_Render(
         IcQuickFboRenderBase::Factory, void*, IcQuickFboRenderBase*
    );
    virtual ~IcQuickFboRenderBase_Render( ) Q_DECL_OVERRIDE;
    virtual void render() Q_DECL_OVERRIDE;
    virtual void synchronize(QQuickFramebufferObject *item) Q_DECL_OVERRIDE;
    QOpenGLFramebufferObject *createFramebufferObject( const QSize &size ) Q_DECL_OVERRIDE
    { return m_item->createFbo_qsg( size ); }
};

// ============================================================================
// ctor
// ============================================================================
IcQuickFboRenderBase_Render :: IcQuickFboRenderBase_Render (
     IcQuickFboRenderBase::Factory cr, void *ctxt, IcQuickFboRenderBase *item
) {
    m_item = item; m_worker = Q_NULLPTR;
    m_fact = cr;   m_fact_ctxt = ctxt;

    if ( m_fact != Q_NULLPTR ) {
        QVariant vr = m_fact( m_fact_ctxt, QStringLiteral("createQSGRenderWorker"), QVariant());
        m_worker = static_cast<QxPack::IcQSGRenderWorker*>( vr.value<void*>());
        if ( m_worker != Q_NULLPTR ) {
            m_worker->initialize();
        }
    }
}

// ============================================================================
// dtor
// ============================================================================
IcQuickFboRenderBase_Render :: ~IcQuickFboRenderBase_Render ( )
{
    if ( m_worker != Q_NULLPTR  &&  m_fact != Q_NULLPTR ) {
        m_fact( m_fact_ctxt, QStringLiteral("deleteQSGRenderWorker"),
            QVariant::fromValue( static_cast<void*>(m_worker))
        );
    }
}

// ============================================================================
// render function
// ============================================================================
void  IcQuickFboRenderBase_Render :: render()
{
    if ( m_worker != Q_NULLPTR ) {
        m_worker->render();        
        if ( m_item != Q_NULLPTR ) {
            // this is recommended by Laszlo Agocs's tutorial
        #if defined( QXPACK_IC_QT5 )
            m_item->window()->resetOpenGLState();
        #else
            QQuickOpenGLUtils::resetOpenGLState();
        #endif
        }
        if ( m_worker->isReqUpdateAfterRender()) {
            this->update();
        }
    }
}

// ===========================================================================
// sync. function
// ===========================================================================
void  IcQuickFboRenderBase_Render :: synchronize(QQuickFramebufferObject *item )
{
    if ( m_worker != Q_NULLPTR ) {
        m_worker->synchronize( item );
    }
}


// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcQuickFboRenderBase :: IcQuickFboRenderBase ( Factory cr_func, void *cr_ctxt, QQuickItem *pa )
    : QQuickFramebufferObject( pa )
{
    m_cr_func = cr_func; m_cr_ctxt = cr_ctxt;
    m_obj = Q_NULLPTR;

    //this->setTextureFollowsItemSize(true);
}

// ============================================================================
// dtor
// ============================================================================
IcQuickFboRenderBase :: ~IcQuickFboRenderBase( )
{

}

// ============================================================================
// create render
// ============================================================================
IcQuickFboRenderBase::Renderer*  IcQuickFboRenderBase :: createRenderer() const
{
    return new IcQuickFboRenderBase_Render(
        m_cr_func, m_cr_ctxt,  const_cast<IcQuickFboRenderBase*>( this )
    );
}

// ============================================================================
// create FBO
// ============================================================================
QOpenGLFramebufferObject *   IcQuickFboRenderBase :: createFbo_qsg(const QSize &size )
{
    QOpenGLFramebufferObject *fbo = Q_NULLPTR;

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    QSize sz = size;
    if ( sz.width() < 64 || sz.height() < 64 ) { sz = QSize(64,64); }
    fbo = new QOpenGLFramebufferObject( sz, format);

    return fbo;
}


}

#endif
