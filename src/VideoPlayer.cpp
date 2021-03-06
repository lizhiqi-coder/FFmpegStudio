//
// Created by 58boy on 2017/9/16.
//

#include "VideoPlayer.h"

#define DELAY(time) std::this_thread::sleep_for(std::chrono::milliseconds(time))

VideoPlayer::VideoPlayer(char *path) {
    video_path = path;
    capturer = new FFmpegCapturer(video_path);
    initUI();
    connect(this, SIGNAL(display(void * , int, int)), surfaceView, SLOT(onRender(void * , int, int)),
            Qt::ConnectionType::BlockingQueuedConnection);
    connect(this, SIGNAL(displayFrame(FFrame * )), surfaceView, SLOT(onRenderFrame(FFrame * )),
            Qt::ConnectionType::BlockingQueuedConnection);
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
            DELAY(100);
            m_state = STATE_PAUSE;
            continue;
        }

        auto frame = capturer->captureFrame();
        if (frame != NULL) {
            if (frame->hasVideo) {

                if (video_frame_queue->size() >= QUEUE_MAX_SIZE) {
                    DELAY(50);
                }
                v_mutex.lock();
                video_frame_queue->push(frame);
                v_mutex.unlock();
            }

            if (frame->hasAudio) {

                if (audio_frame_queue->size() >= QUEUE_MAX_SIZE * 2) {
                    DELAY(50);
                }
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

            video_frame_queue->pop();
            v_mutex.unlock();

            delay = copyFrame->pts - video_state.frame_last_pts;
            if (delay <= 0 || delay >= 1.0) {
                delay = video_state.frame_last_delay;
            }

            video_state.frame_last_delay = delay;
            video_state.frame_last_pts = copyFrame->pts;


            //同步视频到音频
            double ref_clock = get_audio_clock(&video_state);
            double diff = copyFrame->pts - ref_clock;
            double threshold = (delay > AV_SYNC_THRESHOLD) ? delay : AV_NOSYNC_THRESHOLD;

            if (fabs(diff) < AV_NOSYNC_THRESHOLD) {

                if (diff <= -threshold) {// video slow
                    delay = 0;
                }
                if (diff >= threshold) {//video quick
                    delay *= 2;
                }
                video_state.frame_timer += delay;
            }

            DELAY((int64_t) (delay * 1000));

//            printf("video frame pts is %10f,delay is%15f\n", frame->pts, delay);
//            emit display(copyFrame->data, copyFrame->width, copyFrame->height);
            //发射阻塞信号，在 slot 里释放
            emit displayFrame(copyFrame);


        } else {
            DELAY(100);
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

            delay = copyFrame->pts - last_pts;
            last_pts = copyFrame->pts;
//            printf("audio frame pts %10f,delay is %10f\n", copyFrame->pts, delay);
            DELAY((int64_t) (delay * 1000));

            video_state.audio_clock = copyFrame->pts;
            video_state.audio_buf_size = copyFrame->length;

            if (audio_stream != NULL && audio_stream != nullptr) {

                audio_stream->write((const char *) copyFrame->data, copyFrame->length);

                free(copyFrame->data);

            } else {
                printf("audio_stream is null\n");
            }

        } else {
            DELAY(100);
            continue;
        }
    }
    delete copyFrame;
}

void VideoPlayer::initUI() {

    surfaceView = new GLWidget;
    surfaceView->setMinimumSize(capturer->getWidth(), capturer->getHeight());

    controllerBar = new VideoControllerBar;
    auto layout = new QVBoxLayout;
    layout->addWidget(surfaceView);
    layout->addWidget(controllerBar);
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
    int hw_buf_size;//一块音频中尚未播放的缓存大小

    pts = vs->audio_clock;
//    hw_buf_size = vs->audio_buf_size - vs->audio_buf_index;
    hw_buf_size = vs->audio_buf_size / 2;

    if (vs->bytes_per_sec) {
        pts -= (double) hw_buf_size / vs->bytes_per_sec;
    }

    return pts;
}

void VideoPlayer::initVideoState() {
    video_state.samplerate = capturer->getSamplerate();
    video_state.wanted_channels = capturer->getChannels();
    video_state.bytes_per_sec = video_state.samplerate * video_state.wanted_channels * 2;
}

