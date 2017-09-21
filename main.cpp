#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include "gl_widget.h"
#include "src/core/ffmpeg_capturer.h"


int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(application);


    printf("start main\n");
    auto capturer=new FFmpegCapturer("D:\\movie\\av\\javhd\\test.mp4");

    QApplication a(argc, argv);
    GLWidget w;

    w.show();


    while (capturer->captureFrame()) {

//           w.onRender();
    }

    return a.exec();
}
