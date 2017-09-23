//
// Created by admin on 2017/9/21.
//

#ifndef FFMPEGSTUDIO_FFMPEG_CAPTURER_H
#define FFMPEGSTUDIO_FFMPEG_CAPTURER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
};

class FFmpegCapturer {

public:
    FFmpegCapturer(char *video_path);

    ~FFmpegCapturer();

public:
    void start();

    bool captureFrame();

    void processImage();

    void processAudio();



private:
    void release();
    AVPixelFormat getPixFormat();
private:
    double video_time_base;
    double video_frame_rate;

private:
    char *m_video_path;
    AVFormatContext *av_fmt_ctx;
    int video_index = -1;
    int audio_index = -1;
    AVCodecContext *video_codec_ctx;
    AVCodecContext *audio_codec_ctx;
    AVCodec *video_codec, *audio_codec;
    AVPacket *packet;
    AVFrame *video_frame;
    AVFrame *video_RGB_frame;
    AVFrame *audio_frame;

    SwsContext *sws_ctx;

    uint8_t *rgb_frame_buffer;


};


#endif //FFMPEGSTUDIO_FFMPEG_CAPTURER_H
