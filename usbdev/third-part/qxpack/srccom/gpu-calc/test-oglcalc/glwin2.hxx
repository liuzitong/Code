#ifndef GLWIN2_HXX
#define GLWIN2_HXX

#include <QWindow>
#include <QOpenGLFunctions>

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
      Q_OBJECT
public:
      explicit OpenGLWindow( QWindow *parent = Q_NULLPTR );
      virtual ~OpenGLWindow() Q_DECL_OVERRIDE;

      virtual void render(QPainter *painter);
      virtual void render();
      virtual void initialize();

Q_SLOT  void  test();


//protected:
//      bool event(QEvent *event) override;
//      void exposeEvent(QExposeEvent *event) override;

private:
      QOpenGLContext *m_context;
//      QOpenGLPaintDevice *m_device;
};


#endif
