//
// Created by 58boy on 2017/9/7.
//

#include "gl_widget.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {

}


void GLWidget::initializeGL() {

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);
    glEnable(GL_TEXTURE_2D);


    QFile vertex_shader_file(QString(":/res/shader/vertex_shader.glsl"));
    QFile fragment_shader_file(QString(":/res/shader/fragment_shader.glsl"));

    vertex_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
    fragment_shader_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream vertex_in(&vertex_shader_file);
    QTextStream fragment_in(&fragment_shader_file);

    m_program = initProgram(vertex_in.readAll().toLatin1(), fragment_in.readAll().toLatin1());

    m_texture = initTexture(NULL, NULL);

    handle_position = glGetAttribLocation(m_program, KEY_ATTR_POSITION);
    handle_tex_coord = glGetAttribLocation(m_program, KEY_ATTR_TEXCOORD);
    handle_texture = glGetUniformLocation(m_program, KEY_UNI_TEXTURE);

}

GLuint GLWidget::initTexture(int width, int height) {

    glActiveTexture(GL_TEXTURE0);
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    if (texture_id <= 0) {
        printf("create texture failed");
        return 0;
    }
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    QImage qimage;
    if (!qimage.load(":/res/image/test.jpg")) {
        qWarning("load image failed!");
        QImage dummy(128, 128, QImage::Format_RGB32);
        dummy.fill(Qt::red);
        qimage = dummy;
    }

    QImage openglImage = QGLWidget::convertToGLFormat(qimage);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 openglImage.width(), openglImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, openglImage.bits());

    return texture_id;
}

void GLWidget::setCoverPath(const char *cover_path) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    QImage image_buffer;
    image_buffer.load(cover_path);

    auto cover_image = QGLWidget::convertToGLFormat(image_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 cover_image.width(), cover_image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, cover_image.bits());
}


void GLWidget::cleanup() {

    if (m_program > 0) {
        glDeleteProgram(m_program);
    }

    free(texture_buf);
}

void GLWidget::paintGL() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(m_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(handle_texture, 0);


    glVertexAttribPointer(handle_position, 2, GL_FLOAT, false, 0, posCoord);
    glVertexAttribPointer(handle_tex_coord, 2, GL_FLOAT, false, 0, texCoord);
    glEnableVertexAttribArray(handle_position);
    glEnableVertexAttribArray(handle_tex_coord);


    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void GLWidget::resizeGL(int w, int h) {
    glClearColor(1, 1, 1, 1);
    glViewport(0, 0, w, h);
}

void GLWidget::onRender(GLuint texture) {

    m_texture = texture;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(handle_texture, 0);

    update();
}

void GLWidget::onRenderFrame(FFrame *frame) {

    free(texture_buf);
    texture_buf = static_cast<BYTE *>(malloc(frame->length * sizeof(BYTE)));
    memset(texture_buf, 0, frame->length);
    texture_width = frame->width;
    texture_height = frame->height;
    memcpy(texture_buf, frame->data, frame->length);
    free(frame->data);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glViewport(0, 0, texture_width, texture_height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_buf);
    update();
}


void GLWidget::onRender(void *texture_data, int width, int height) {

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glViewport(0, 0, width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    update();
}


/**********************************************************************************/
GLint GLWidget::initProgram(const char *vertex_shader_code,
                            const char *fragment_shader_code) {

    GLint program;

    auto vertex_shader_id = compileShader(GL_VERTEX_SHADER, vertex_shader_code);
    auto fragment_shader_id = compileShader(GL_FRAGMENT_SHADER, fragment_shader_code);

    program = glCreateProgram();

    if (program == 0) {
        printf("create program failed");
        return 0;
    }
    glAttachShader(program, vertex_shader_id);
    glAttachShader(program, fragment_shader_id);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == 0) {
        glDeleteProgram(program);
        program = 0;
    }

    return program;
}

GLint GLWidget::compileShader(GLenum type, const char *shader_code) {
    auto shader_obj_id = glCreateShader(type);
    if (shader_obj_id == 0) {
        return 0;
    }
    glShaderSource(shader_obj_id, 1, &shader_code, NULL);
    glCompileShader(shader_obj_id);
    GLint status = -1;
    glGetShaderiv(shader_obj_id, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(shader_obj_id);
        shader_obj_id = 0;
    }
    return shader_obj_id;

}

GLWidget::~GLWidget() {
    cleanup();
}






