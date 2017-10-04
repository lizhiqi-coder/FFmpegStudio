//
// Created by 58boy on 2017/9/16.
//

#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(char *path) {
    video_path = path;
    capturer = new FFmpegCapturer(video_path);
    initUI();

    connect(this, SIGNAL(display(void * , int, int)), surfaceView, SLOT(onRender(void * , int, int)));

    capture_thread = new std::thread(VideoPlayer::capture_runnable, this);
    display_thread = new std::thread(VideoPlayer::display_runnable, this);

}

VideoPlayer::~VideoPlayer() {

    if (capturer != nullptr) {
        delete capturer;
        capturer = nullptr;
    }

    if (surfaceView != nullptr) {
        delete surfaceView;
        surfaceView = nullptr;
    }
    if (capture_thread != nullptr) {
        delete capture_thread;
        capture_thread = nullptr;
    }
    if (display_thread != nullptr) {
        delete display_thread;
    }

}

void VideoPlayer::play() {

    if (m_state == STATE_STOP) {

        m_state = STATE_PLAYING;

        capture_thread->detach();
        display_thread->detach();

    }

    do_play = true;
    do_pause = false;
    do_stop = false;

}

void VideoPlayer::pause() {

    do_pause = true;
    do_play = false;
    do_stop = false;


}

void VideoPlayer::stop() {
    do_stop = true;
    do_play = false;
    do_pause = false;

}

void VideoPlayer::capture_runnable() {

    while (m_state != STATE_STOP) {
        printf("capture_runnable\n");

        if (do_stop) {
            m_state = STATE_STOP;
            break;
        }
        if (do_pause) {
//            毫秒
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            m_state = STATE_PAUSE;
            continue;
        }

        auto frame_ptr = capturer->captureFrame();
        if (frame_ptr != NULL) {
            video_frame_queue.push(frame_ptr);
        }
    }
}

void VideoPlayer::display_runnable() {

    while (m_state != STATE_STOP) {
        printf("display_runnable\n");

        if (video_frame_queue.size() > 0) {

            auto frame = video_frame_queue.front();
            emit display(frame->data, frame->width, frame->height);

            video_frame_queue.pop();
            std::this_thread::sleep_for(std::chrono::milliseconds(35));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

    }

}

void VideoPlayer::initUI() {

    surfaceView = new GLWidget;
    auto layout = new QVBoxLayout;
    layout->addWidget(surfaceView);

    setLayout(layout);

}
