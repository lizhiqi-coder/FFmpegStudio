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
    initAudioPlayer(44100, 2);
    initVideoState();
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
    if (video_thread != nullptr) {
        delete video_thread;
        video_thread = nullptr;
    }
    if (audio_thread != nullptr) {
        delete audio_thread;
        audio_thread = nullptr;
    }

    delete video_frame_queue;
    delete audio_frame_queue;
    video_frame_queue = nullptr;
    audio_frame_queue = nullptr;

    av_free(audio);
    av_free(audio_stream);
    av_free(video_path);
}

void VideoPlayer::play() {

    if (m_state == STATE_STOP) {

        if (capture_thread != nullptr) {
            delete capture_thread;
        }
        if (video_thread != nullptr) {
            delete video_thread;
        }

        m_state = STATE_PLAYING;
        capture_thread = new std::thread(VideoPlayer::capture_runnable, this);
        video_thread = new std::thread(VideoPlayer::video_runnable, this);
        audio_thread = new std::thread(VideoPlayer::audio_runnable, this);
        capture_thread->detach();
        video_thread->detach();
        audio_thread->detach();

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
            if (frame->hasVideo) {

                v_mutex.lock();
                video_frame_queue->push(frame);
                v_mutex.unlock();
            }

            if (frame->hasAudio) {
                a_mutex.lock();
                audio_frame_queue->push(frame);
                a_mutex.unlock();
            }

        } else {
            m_state = STATE_STOP;
        }
    }
}

void VideoPlayer::video_runnable() {
    double delay = 0;//second

    FFrame *copyFrame = new FFrame();
    while (m_state != STATE_STOP) {

        if (video_frame_queue->size() > 0) {
            v_mutex.lock();

            auto frame = video_frame_queue->front();
            copyFrame->copy(frame);
            delete frame;


            delay = copyFrame->pts - video_state.frame_last_pts;
            if (delay <= 0 || delay >= 1.0) {
                delay = video_state.frame_last_delay;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds((int64_t) (delay * 1000)));
//                printf("frame pts is %10f,delay is%15f\n", frame->pts, delay);
            emit display(copyFrame->data, copyFrame->width, copyFrame->height);

            video_state.frame_last_delay = delay;
            video_state.frame_last_pts = copyFrame->pts;

            video_frame_queue->pop();
            v_mutex.unlock();

        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

    }
    delete copyFrame;

}

void VideoPlayer::audio_runnable() {
    double delay = 0;
    double last_pts = 0;
    FFrame *copyFrame = new FFrame();

    while (m_state != STATE_STOP) {
        if (audio_frame_queue->size() > 0) {
            a_mutex.lock();
            auto frame = audio_frame_queue->front();

            copyFrame->copy(frame);
            delete frame;
            audio_frame_queue->pop();
            a_mutex.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(21));

            if (audio_stream != NULL && audio_stream != nullptr) {

                audio_stream->write((const char *) copyFrame->data, copyFrame->length);

            } else {
                printf("audio_stream is null\n");
            }

        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
    }
    delete copyFrame;
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

double VideoPlayer::get_audio_clock(VideoState *vs) {

    double pts;
    int hw_buf_size;

    pts = vs->audio_clock;
    hw_buf_size = vs->audio_buf_size - vs->audio_buf_index;

    if (vs->bytes_per_sec) {
        pts = (double) hw_buf_size / vs->bytes_per_sec;
    }

    return pts;
}

void VideoPlayer::initVideoState() {
    video_state.samplerate = capturer->getSamplerate();
    video_state.wanted_channels = capturer->getChannels();
    video_state.bytes_per_sec = video_state.samplerate * video_state.wanted_channels * 2;
}

