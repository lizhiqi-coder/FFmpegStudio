//
// Created by 58boy on 2017/9/7.
//

#include "gl_widget.h"

void GLWidget::initializeGL() {

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);
    initProgram();
}


void GLWidget::cleanup() {

}

bool GLWidget::initProgram() {
//    m_program = new QOpenGLShaderProgram;
//
//    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, "vertex shader");
//    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, "fragment shader");

    m_program = glCreateProgram();


    return false;
}

void GLWidget::paintEvent(QPaintEvent *e) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_program);
}

void GLWidget::resizeGL(int w, int h) {
    glClearColor(1, 1, 1, 1);
    glViewport(0, 0, w, h);
}


