//
// Created by admin on 2017/9/21.
//

#include "ffmpeg_capturer.h"

FFmpegCapturer::FFmpegCapturer(char *video_path) : m_video_path(video_path) {

    av_register_all();
    avcodec_register_all();

    av_fmt_ctx = avformat_alloc_context();

    int ret;
    ret = avformat_open_input(&av_fmt_ctx, m_video_path, NULL, NULL);

    printf("opengl video result :%d\n", ret);

    if (ret < 0) {
        printf("open video failed %d\n", ret);
        return;
    }
    for (int i = 0; i < av_fmt_ctx->nb_streams; ++i) {

        if (av_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
        }
        if (av_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_index = i;
        }
    }

    if (video_index < 0 || audio_index < 0) {
        printf("can not find video index or audio index\n");
        return;
    }

    video_time_base = av_q2d(av_fmt_ctx->streams[video_index]->time_base);
    video_frame_rate = av_q2d(av_fmt_ctx->streams[video_index]->r_frame_rate);


    video_codec_ctx = av_fmt_ctx->streams[video_index]->codec;
    audio_codec_ctx = av_fmt_ctx->streams[audio_index]->codec;
    video_codec = avcodec_find_decoder(video_codec_ctx->codec_id);
    audio_codec = avcodec_find_decoder(audio_codec_ctx->codec_id);

    if (avcodec_open2(video_codec_ctx, video_codec, NULL) < 0) {
        printf("open video codec failed\n");
    }
    if (avcodec_open2(audio_codec_ctx, audio_codec, NULL) < 0) {
        printf("open audio codec failed\n");
    }


    int picture_size = video_codec_ctx->width * video_codec_ctx->height;
    packet = (AVPacket *) malloc(sizeof(AVPacket));
    av_new_packet(packet, picture_size);

    video_frame = av_frame_alloc();
    video_RGB_frame = av_frame_alloc();
    audio_frame = av_frame_alloc();

    sws_ctx = sws_getContext(video_codec_ctx->width, video_codec_ctx->height, getPixFormat(),
                             video_codec_ctx->width, video_codec_ctx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC,
                             NULL, NULL, NULL);

    int rgb_picture_size = avpicture_get_size(AV_PIX_FMT_RGB24, video_codec_ctx->width, video_codec_ctx->height);
    rgb_frame_buffer = (uint8_t *) (av_malloc(rgb_picture_size * sizeof(uint8_t)));
    avpicture_fill((AVPicture *) video_RGB_frame, rgb_frame_buffer, AV_PIX_FMT_RGB24,
                   video_codec_ctx->width, video_codec_ctx->height);

    av_dump_format(av_fmt_ctx, 0, m_video_path, 0);


}

FFmpegCapturer::~FFmpegCapturer() {
    release();
}

void FFmpegCapturer::start() {

}

bool FFmpegCapturer::captureFrame() {

    int ret;
    int got_picture;
    int got_frame;

    ret = av_read_frame(av_fmt_ctx, packet);
    if (ret < 0) {
        printf("read no frame\n");
        return false;
    }

//    video
    if (packet->stream_index == video_index) {

        ret = avcodec_decode_video2(video_codec_ctx, video_frame, &got_picture, packet);
        if (ret < 0) {
            printf("decode video failed\n");
        }

        if (got_picture) {

            auto pts = av_frame_get_best_effort_timestamp(video_frame);
            auto timestamp = video_time_base * pts;
//            timestamp*video_frame_rate/
            video_frame->key_frame;
//            video_frame.
            processImage();

        }
    }


//    audio
    if (packet->stream_index == audio_index) {
        ret = avcodec_decode_audio4(audio_codec_ctx, audio_frame, &got_frame, packet);
        if (ret < 0) {
            printf("decode audio failes\n");
        }
        if (got_picture) {
            printf("got audio frame\n");
            processAudio();
        }
    }
    return true;

}

void FFmpegCapturer::release() {

    if (packet->size > 0) {
        av_packet_unref(packet);
        packet->size = 0;
    }

    if (video_frame != NULL) {
        av_frame_free(&video_frame);
        video_frame = NULL;
    }
    if (video_RGB_frame != NULL) {
        av_frame_free(&video_RGB_frame);
        video_RGB_frame = NULL;
    }

    if (audio_frame != NULL) {
        av_frame_free(&audio_frame);
        audio_frame = NULL;
    }

    if (video_codec_ctx != NULL) {
        avcodec_close(video_codec_ctx);
        video_codec_ctx = NULL;
    }
    if (audio_codec_ctx != NULL) {
        avcodec_close(audio_codec_ctx);
        video_codec_ctx = NULL;
    }
    if (rgb_frame_buffer) {
        av_free(rgb_frame_buffer);
        rgb_frame_buffer = NULL;
    }

    if (av_fmt_ctx != NULL) {
        avformat_close_input(&av_fmt_ctx);
        av_fmt_ctx = NULL;
    }
    if (sws_ctx != NULL) {
        sws_freeContext(sws_ctx);
        sws_ctx = NULL;
    }


}

void FFmpegCapturer::processImage() {
    sws_ctx = sws_getCachedContext(sws_ctx, video_codec_ctx->width, video_codec_ctx->height,
                                   video_codec_ctx->pix_fmt, video_codec_ctx->width, video_codec_ctx->height,
                                   AV_PIX_FMT_RGBA,
                                   SWS_BICUBIC, NULL, NULL, NULL);

}

void FFmpegCapturer::processAudio() {

}

AVPixelFormat FFmpegCapturer::getPixFormat() {

    if (video_codec_ctx != NULL && video_codec_ctx->pix_fmt != AV_PIX_FMT_NONE) {
        return video_codec_ctx->pix_fmt;
    }
    return AV_PIX_FMT_RGB24;

}


