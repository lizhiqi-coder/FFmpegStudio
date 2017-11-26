#-------------------------------------------------
#
# Project created by QtCreator 2017-09-02T18:24:11
#
#-------------------------------------------------

QT       += core gui opengl multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FFmpegStudio
TEMPLATE = app

#console test
CONFIG += console
#set c++ version for debug model
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
            mainwindow.cpp \
            src/core/h264_decoder.cpp \
            src/core/camera_client.cpp\
            src/opengl/gl_widget.cpp\
            src/VideoPlayer.cpp\
            src/core/ffmpeg_capturer.cpp\
            src/core/ffmpeg_frame.cpp\
            src/windows/video_controller_bar.cpp\
            src/core/streamer/video_streamer.cpp
HEADERS  += mainwindow.h \
            src/core/h264_decoder.h \
            src/core/h264_frame.h   \
            src/core/camera_client.h \
            src/opengl/gl_widget.h \
            src/VideoPlayer.h   \
            src/core/ffmpeg_capturer.h\
            src/core/ffmpeg_frame.h\
            src/windows/video_controller_bar.h\
            src/core/streamer/video_streamer.h



FORMS    += mainwindow.ui \
    res/ui/openglwindow.ui \
    res/ui/video_controller_bar.ui

RESOURCES += application.qrc \

win32{

INCLUDEPATH += $$PWD/include
INCLUDEPATH +=$$PWD/src/core
INCLUDEPATH +=$$PWD/src/opengl

LIBS += -L$$PWD/lib
LIBS += -lavcodec \
        -lavformat \
        -lavdevice \
        -lavfilter \
        -lavutil \
        -lswresample \
        -lswscale

}
