//
// Created by 58boy on 2017/9/16.
//

#ifndef FFMPEGSTUDIO_VIDEOPLAYER_H
#define FFMPEGSTUDIO_VIDEOPLAYER_H


extern "C" {
#include "libavutil/frame.h"
};

#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <QtWidgets/QWidget>
#include <queue>

#include "opengl/gl_widget.h"
#include "core/ffmpeg_capturer.h"
#include "core/ffmpeg_frame.h"

#define QUEUE_MAX_SIZE 8

class VideoPlayer : public QWidget {

Q_OBJECT
public:
    VideoPlayer(char *path);

    ~VideoPlayer();

signals:

    void display(void *data, int w, int h);

public:
    void play();

    void pause();

    void stop();

    void seekTo(double timestamp);

private:
    const static int STATE_STOP = 0;
    const static int STATE_PLAYING = 1;
    const static int STATE_PAUSE = 2;

    volatile int m_state = STATE_STOP;

    bool do_play = false;
    bool do_pause = false;
    bool do_stop = false;

    void initUI();

    void capture_runnable();

    void display_runnable();

private:
    char *video_path;
    FFmpegCapturer *capturer;
    GLWidget *surfaceView;


private:

    std::queue<FFrame *> *video_frame_queue;
    std::queue<FFrame *> *audio_frame_queue;

    std::thread *capture_thread;
    std::thread *display_thread;
    std::mutex mutex;

};


#endif //FFMPEGSTUDIO_VIDEOPLAYER_H
