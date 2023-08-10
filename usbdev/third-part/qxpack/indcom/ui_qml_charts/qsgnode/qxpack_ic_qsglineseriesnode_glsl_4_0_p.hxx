#ifndef QXPACK_IC_QSGLINESERIESNODE_GLSL_4_0_P_HXX
#define QXPACK_IC_QSGLINESERIESNODE_GLSL_4_0_P_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack_ic_qsglineseriesnode_p.hxx"
#include "qxpack/indcom/ui_qml_base/qxpack_ic_qsgoglshaderhlp_p.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                   the line series node ( GLSL 4.0/GLSL ES 4.0 )
// Warning: the GLSL ES 3.0 is not tested! just test on GLSL 4.0
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcQSGLineSeriesNodeGLSL4_0 : public IcQSGLineSeriesNode {
private:
    IcQSGOglShaderHlp  m_sdr_hlp;

    size_t  m_vbo_buff_size;
    GLuint  m_vec_prog_id, m_lst_prog_id, m_vbo_id, m_ubo_id;
    GLint   m_vec_uni_color_loc, m_lst_uni_color_loc;
    GLuint  m_vec_unival_idx, m_lst_unival_idx; // just for GLSL 4.0/GLSL ES 3.0

    GLuint  m_rt_prev_vbo_id, m_rt_prev_ubo_id;
    qreal   m_rt_curr_disp_w, m_rt_curr_disp_h;
    size_t  m_rt_curr_data_cnt;

    bool    m_is_init, m_is_ogl_es;
protected:
    auto   setupUniform() -> void;
    auto   bindAndUploadUBO( const RenderState *state ) -> void;
    auto   bindAndUploadVBO_Vec( ) -> void;
    auto   bindAndUploadVBO_Lst( ) -> void;
    auto   ensureUBO( bool ) -> void;
    auto   ensureVBO( bool ) -> void;
    auto   initResoruces()   -> void;
public :
    explicit IcQSGLineSeriesNodeGLSL4_0(  QQuickItem *item );
    virtual ~IcQSGLineSeriesNodeGLSL4_0() Q_DECL_OVERRIDE;

    virtual auto  render(const RenderState *state) -> void  Q_DECL_OVERRIDE;
    virtual auto  releaseResources()    -> void       Q_DECL_OVERRIDE;
    virtual auto  changedStates() const -> StateFlags Q_DECL_OVERRIDE;
    virtual auto  flags() const -> RenderingFlags     Q_DECL_OVERRIDE;
    virtual auto  rect()  const -> QRectF             Q_DECL_OVERRIDE;
};


}

#endif
