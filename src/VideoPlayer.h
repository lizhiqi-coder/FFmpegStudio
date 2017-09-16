//
// Created by 58boy on 2017/9/16.
//

#ifndef FFMPEGSTUDIO_VIDEOPLAYER_H
#define FFMPEGSTUDIO_VIDEOPLAYER_H

#include <QtCore/QThread>
#include <stdio.h>

extern  "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
};

class VideoPlayer :public QThread {
    Q_OBJECT
public:
    explicit VideoPlayer(std::string path);

    ~VideoPlayer();

private:
    int init();

    void grabFrame();

    void saveFrame(AVFrame *frame, int w, int h);
protected:
    void run();

private:
    std::string video_path;

    AVFormatContext *avfmtCtx;

    int video_stream_index=-1;
    int audio_stream_index = -1;

    AVCodecContext *avCodecCtx;
    AVCodec *videoCodec;

    int imageWidth, imageHeight;
    int image_size;
    AVPacket *packet;
    AVFrame*video_frame;
    AVFrame*video_RGB_frame;
    AVFrame*audio_frame;

    SwsContext *image_convert_ctx;

    uint8_t *out_buffer;
    int numBytes;


};


#endif //FFMPEGSTUDIO_VIDEOPLAYER_H
