#ifndef QXPACK_IC_QSGVALUEAXISNODE_GLSL_4_3_P_HXX
#define QXPACK_IC_QSGVALUEAXISNODE_GLSL_4_3_P_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack_ic_qsgvalueaxisnode_p.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_qsgoglshaderhlp_p.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//               value axisnode GLSL 4.3
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcQSGValueAxisNodeGLSL4_3 : public IcQSGValueAxisNode {
private:
    IcQSGOglShaderHlp  m_sdr_hlp;

    size_t  m_vbo_buff_size;
    GLuint  m_prog_id, m_vbo_id, m_ubo_id;
    GLint   m_uni_color_loc;

    bool    m_is_init, m_is_ogl_es;
public :
    explicit IcQSGValueAxisNodeGLSL4_3(  QQuickItem *item );
    virtual ~IcQSGValueAxisNodeGLSL4_3() Q_DECL_OVERRIDE;

    virtual auto  render(const RenderState *state) -> void  Q_DECL_OVERRIDE;
    virtual auto  releaseResources()    -> void       Q_DECL_OVERRIDE;
    virtual auto  changedStates() const -> StateFlags Q_DECL_OVERRIDE;
    virtual auto  flags() const -> RenderingFlags     Q_DECL_OVERRIDE;
    virtual auto  rect()  const -> QRectF             Q_DECL_OVERRIDE;
};

}

#endif
