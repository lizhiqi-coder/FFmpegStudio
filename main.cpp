#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include "gl_widget.h"
#include "src/core/ffmpeg_capturer.h"


int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(application);


    printf("start main\n");
    auto capturer = new FFmpegCapturer("C:\\Users\\58boy\\Desktop\\daoxiang.mp4");

    QApplication a(argc, argv);
    GLWidget w;
    w.show();
    w.setCoverPath(":\\res\\image\\iron_man.jpg");

    AVFrame *video_frame;
    int frame_index = 0;

    do {

        video_frame = capturer->captureFrame();

        if (video_frame != NULL) {

            printf("capture frame: %d\n", frame_index);
            frame_index++;
        }
        if (frame_index > 100) {
            break;
        }

    } while (1);


    printf("video frame width : %d\n", video_frame->width);
    printf("video frame height : %d\n", video_frame->height);

    w.onRender(video_frame->data[0], video_frame->width, video_frame->height);


    return a.exec();
}
