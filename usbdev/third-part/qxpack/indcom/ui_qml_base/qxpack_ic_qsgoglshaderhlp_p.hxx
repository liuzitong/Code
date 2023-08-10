#ifndef QXPACK_IC_QSGOGLSHADERHLP_P_HXX
#define QXPACK_IC_QSGOGLSHADERHLP_P_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QByteArray>
#include <QOpenGLExtraFunctions>
#include <QString>

namespace QxPack {

class QXPACK_IC_HIDDEN IcQSGOglShaderHlp {
private:
    void *m_obj;
public :
    static auto   compileShader (
        const QByteArray &shader, QByteArray &info_log, GLenum sdr_type
    ) -> GLuint;

    static auto   createProgram (
        GLuint vtx_sd, GLuint geom_sd, GLuint frag_sd,  QByteArray  &info_log,
        void (* LinkBefore )( void *ctxt, GLuint prog_id ), void *ctxt
    ) -> GLuint;

    explicit IcQSGOglShaderHlp( );
    ~IcQSGOglShaderHlp( );

    auto   tryAddRefProgram ( const QString  & prog_name ) -> GLuint;
    auto   takeAddProgram   ( const QString  & prog_name, GLuint prog_id ) -> bool;
    auto   decRefProgram    ( const QString  & prog_name ) -> void;
};

}
#endif
