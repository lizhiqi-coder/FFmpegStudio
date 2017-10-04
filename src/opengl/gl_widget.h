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

    void setCoverPath(const char *cover_path);

private:
    GLint initProgram(const char *vertex_shader_code,
                      const char *fragment_shader_code);

    GLuint initTexture(int width, int height);

    GLint compileShader(GLenum type, const char *shader_code);


protected:
    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int w, int h) override;

public:
    void onRender(GLuint texture);

public slots:

    void onRender(void *texture_data, int width, int height);

    void cleanup();

private:
    GLint m_program;
    GLuint m_texture;

    GLint handle_position;
    GLint handle_tex_coord;
    GLint handle_texture;

private:
    const float posCoord[8] = {
            -1, 1,
            1, 1,
            -1, -1,
            1, -1
    };
    const float texCoord[8] = {
            0, 1,
            1, 1,
            0, 0,
            1, 0
    };
};


#endif //FFMPEGSTUDIO_GLWIDGET_H
