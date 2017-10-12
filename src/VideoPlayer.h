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
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioOutput>
#include <queue>

#include "opengl/gl_widget.h"
#include "core/ffmpeg_capturer.h"
#include "core/ffmpeg_frame.h"

#define QUEUE_MAX_SIZE 8
#define AV_SYNC_THRESHOLD 0.01


class PacketQueue {

public:
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    std::mutex mutex;
};


class VideoState {
public:
    int video_index;
    int audio_index;
    int frame_finished;
    int wanted_freq;
    int wanted_samples;
    int wanted_channels;
    int picture_size, picture_rindex, picture_windex;
    unsigned int audio_buf_size;
    unsigned int audio_buf_index;
    double frame_timer;
    double frame_last_pts = 0;
    double now_video_pts;
    double frame_last_delay = 0;
    double video_clock;
    double audio_clock = 0;

    int bytes_per_sec;
};

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

    void initAudioPlayer(int samplerate, int channels);

    void capture_runnable();

    void display_runnable();

    void audio_runnable();

    double get_audio_clock(VideoState *vs);

private:
    char *video_path;
    FFmpegCapturer *capturer;
    GLWidget *surfaceView;
    QAudioOutput *audio;
    QIODevice *audio_stream;

    VideoState video_state;


private:

    std::queue<FFrame *> *video_frame_queue;
    std::queue<FFrame *> *audio_frame_queue;

    std::thread *capture_thread;
    std::thread *display_thread;
    std::thread *audio_thread;
    std::mutex mutex;
    std::mutex a_mutex;

};


#endif //FFMPEGSTUDIO_VIDEOPLAYER_H
