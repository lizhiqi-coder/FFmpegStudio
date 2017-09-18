//
// Created by 58boy on 2017/9/7.
//

#ifndef FFMPEGSTUDIO_GLWIDGET_H
#define FFMPEGSTUDIO_GLWIDGET_H

#include <QtWidgets/QWidget>
#include <QtOpenGL/QtOpenGL>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {

Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);

    ~GLWidget();

private:
    bool initProgram();
protected:
    void initializeGL() override;

    void paintEvent(QPaintEvent *e) override;

    void resizeGL(int w, int h) override;

public slots:

    void cleanup();

private:
    QOpenGLShaderProgram *program;
    GLint  m_program;

};


#endif //FFMPEGSTUDIO_GLWIDGET_H
