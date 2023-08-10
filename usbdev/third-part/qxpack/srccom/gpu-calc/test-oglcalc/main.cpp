/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include <QSurfaceFormat>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QDebug>
#include <QPair>

#include "../oglcalc/qtoglfuncbroker.hxx"
#include "glwin2.hxx"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);

    // Request OpenGL ES 3.1 context, as this is a GLES example. If not available, go for OpenGL 4.3 core.
    if ( OglCalc::QtOglFuncBroker::isSupport(3,1,true) ) {
        qDebug("Requesting 3.1 GLES context");
        fmt.setVersion(3, 1);
        fmt.setRenderableType(QSurfaceFormat::OpenGLES);
    } else if ( OglCalc::QtOglFuncBroker::isSupport( 4,3,false,QSurfaceFormat::CoreProfile)) {
        qDebug("Requesting 4.3 core context");
        fmt.setVersion(4, 3);
        fmt.setRenderableType(QSurfaceFormat::OpenGL);
        fmt.setProfile(QSurfaceFormat::CoreProfile);
    } else {
        qWarning("Error: This system does not support OpenGL Compute Shaders! Exiting.");
        return -1;
    }
    QSurfaceFormat::setDefaultFormat(fmt);


    OpenGLWindow win;
   // win.setFlags ( Qt::SplashScreen );
    win.setX(0); win.setY(0);
    win.setWidth (100);
    win.setHeight(100);
    win.show();



    return app.exec();
}
