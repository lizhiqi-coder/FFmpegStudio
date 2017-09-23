#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include "gl_widget.h"
#include "src/core/ffmpeg_capturer.h"


int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(application);


    printf("start main\n");
    auto capturer=new FFmpegCapturer("C:\\Users\\58boy\\Desktop\\daoxiang.mp4");

    QApplication a(argc, argv);
    GLWidget w;
    w.show();
    w.setCoverPath(":\\res\\image\\iron_man.jpg");


//    while (capturer->captureFrame()) {
//
////           w.onRender();
//    }

    return a.exec();
}
