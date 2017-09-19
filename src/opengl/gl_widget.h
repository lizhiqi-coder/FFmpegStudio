//
// Created by 58boy on 2017/9/7.
//

#ifndef FFMPEGSTUDIO_GLWIDGET_H
#define FFMPEGSTUDIO_GLWIDGET_H

#include <QtWidgets/QWidget>
#include <QtOpenGL/QtOpenGL>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>

#define KEY_ATTR_POSITION  "aPosition"
#define KEY_ATTR_TEXCOORD "aTextureCoord"
#define KEY_UNI_TEXTURE "uTexture"


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {

Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);

    ~GLWidget();

private:
    GLint initProgram(const char *vertex_shader_code,
                      const char *fragment_shader_code);

    GLint compileShader(GLenum type, const char *shader_code);


protected:
    void initializeGL() override;

    void paintEvent(QPaintEvent *e) override;

    void resizeGL(int w, int h) override;

public slots:

    void cleanup();

private:
    GLint m_program;
    GLint m_texture_id;

    GLint handle_position;
    GLint handle_tex_coord;
    GLint handle_texture;

private:
    const static float posCoord[] = {
            -1, 1,
            1, 1,
            -1, -1,
            1, -1
    };
    const static float texCoord[] = {
            0, 1,
            1, 1,
            0, 0,
            1, -0
    };
};


#endif //FFMPEGSTUDIO_GLWIDGET_H
