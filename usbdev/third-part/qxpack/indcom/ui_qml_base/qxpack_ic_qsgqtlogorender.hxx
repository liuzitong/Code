// port and modified from QT5's example
#ifndef QXPACK_IC_QSGQTLOGORENDER_HXX
#define QXPACK_IC_QSGQTLOGORENDER_HXX

#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QTime>
#include <QVector>

#include "qxpack/indcom/ui_qml_base/qxpack_ic_qsgrenderworker.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief this class used to draw a 3D Q character
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcQSGQtLogoRender : public IcQSGRenderWorker, protected QOpenGLFunctions
{
public:
    //! ctor
    explicit IcQSGQtLogoRender();

    //! dtor
    virtual ~IcQSGQtLogoRender() Q_DECL_OVERRIDE;

    //! initailize all things
    virtual bool initialize() Q_DECL_OVERRIDE;

    //! do the render work
    virtual void render() Q_DECL_OVERRIDE;

    virtual bool isReqUpdateAfterRender() Q_DECL_OVERRIDE { return true; }

private:
    qreal   m_fAngle;
    qreal   m_fScale;

    void paintQtLogo();
    void createGeometry();
    void quad(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4);
    void extrude(qreal x1, qreal y1, qreal x2, qreal y2);

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QOpenGLShaderProgram program1;
    int vertexAttr1;
    int normalAttr1;
    int matrixUniform1;
};


}
#endif
