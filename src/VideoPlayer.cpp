//
// Created by 58boy on 2017/9/16.
//

#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(char *path) {
    video_path = path;
    capturer = new FFmpegCapturer(video_path);
    initUI();

    connect(this, SIGNAL(display(void * , int, int)), surfaceView, SLOT(onRender(void * , int, int)));
    video_frame_queue = new std::queue<FFrame *>();
    audio_frame_queue = new std::queue<FFrame *>();
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
        display_thread = nullptr;
    }

    delete video_frame_queue;
    delete audio_frame_queue;
    video_frame_queue = nullptr;
    audio_frame_queue = nullptr;

    av_free(audio);
    av_free(audio_stream);
}

void VideoPlayer::play() {

    if (m_state == STATE_STOP) {

        if (capture_thread != nullptr) {
            delete capture_thread;
        }
        if (display_thread != nullptr) {
            delete display_thread;
        }

        m_state = STATE_PLAYING;
        capture_thread = new std::thread(VideoPlayer::capture_runnable, this);
        display_thread = new std::thread(VideoPlayer::display_runnable, this);

        capture_thread->detach();
        display_thread->detach();

        this->initAudioPlayer(44100, 2);
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

        auto frame = capturer->captureFrame();
        if (frame != NULL) {

            mutex.lock();
            video_frame_queue->push(frame);
            mutex.unlock();

        } else {
            m_state = STATE_STOP;
        }
    }
}

void VideoPlayer::display_runnable() {
    int64_t delta = 0;//微秒
    double time = 0;
    while (m_state != STATE_STOP) {

        if (video_frame_queue->size() > 0) {
            mutex.lock();

            auto frame = video_frame_queue->front();

            if (frame->hasVideo) {

                if (time < frame->pts) {
                    delta = (frame->pts - time) * 1000;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(delta));

                emit display(frame->data, frame->width, frame->height);

                time = frame->pts;
            }

            if (frame->hasAudio) {

                if (audio_stream != NULL && audio_stream != nullptr) {

                    audio_stream->write((const char *) frame->data, frame->length);
                    delete frame;
                    frame = nullptr;

                } else {

                    printf("audio_stream is null\n");
                }
            }

            video_frame_queue->pop();
            mutex.unlock();
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

/**
 * 只调用一次
 * @param samplerate
 * @param channels
 */
void VideoPlayer::initAudioPlayer(int samplerate, int channels) {

    QAudioFormat audioFormat;
    audioFormat.setSampleRate(samplerate);
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    audioFormat.setChannelCount(channels);
    audioFormat.setSampleSize(16);//short
    audioFormat.setCodec("audio/pcm");

    auto deviceInfo = QAudioDeviceInfo::defaultInputDevice();
    if (!deviceInfo.isFormatSupported(audioFormat)) {
        printf("default format is not supported\n");
        audioFormat = deviceInfo.nearestFormat(audioFormat);
    }


    audio = new QAudioOutput(audioFormat, QApplication::instance());
    audio_stream = audio->start();
    printf("audio start\n");
}
