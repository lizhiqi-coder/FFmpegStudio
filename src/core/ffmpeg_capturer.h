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
#include "libswresample/swresample.h"
};

#include "ffmpeg_frame.h"

#define NB_SAMPLES 1024
#define MAX_AUDIO_FRAME_SZIE 192000

class FFmpegCapturer {

public:
    FFmpegCapturer(char *video_path);

    ~FFmpegCapturer();

public:

    FFrame *captureFrame();


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
    SwrContext *swr_ctx;

    uint8_t *rgb_frame_buffer;
    BYTE *frame_buffer;


    int rgb_picture_size;
    double current_pts = 0;

    /**audio**/
    int out_audio_samplerate = 0;
    int out_audio_channels = 0;
    AVSampleFormat out_audio_sample_fmt = AV_SAMPLE_FMT_S16;
    int out_audio_length;


};


#endif //FFMPEGSTUDIO_FFMPEG_CAPTURER_H
