//
// Created by admin on 2017/9/21.
//

#include "ffmpeg_capturer.h"

FFmpegCapturer::FFmpegCapturer(char *video_path) : m_video_path(video_path) {

    av_register_all();
    avcodec_register_all();

    av_fmt_ctx = avformat_alloc_context();

    if (avformat_open_input(&av_fmt_ctx, m_video_path, NULL, NULL) < 0) {
        printf("open video failed ");
        return;
    }
    for (int i = 0; i < av_fmt_ctx->nb_streams; ++i) {

        if (av_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
        }
        if (av_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_index = i;
        }

        if (video_index < 0 || audio_index < 0) {
            printf("can not find video index or audio index");
            return;
        }
        video_codec_ctx = av_fmt_ctx->streams[video_index]->codec;
        audio_codec_ctx = av_fmt_ctx->streams[audio_index]->codec;
        video_codec = avcodec_find_decoder(video_codec_ctx->codec_id);
        audio_codec = avcodec_find_decoder(audio_codec_ctx->codec_id);

        if (avcodec_open2(video_codec_ctx, video_codec, NULL) < 0) {
            printf("open video codec failed");
        }
        if (avcodec_open2(audio_codec_ctx, audio_codec, NULL) < 0) {
            printf("opengl audio codec failed");
        }


        int picture_size = video_codec_ctx->width * video_codec_ctx->height;
        packet = (AVPacket *) malloc(sizeof(packet));
        av_new_packet(packet, picture_size);

        video_frame = av_frame_alloc();
        video_RGB_frame = av_frame_alloc();
        audio_frame = av_frame_alloc();

        sws_ctx = sws_getContext(video_codec_ctx->width, video_codec_ctx->height, video_codec_ctx->pix_fmt,
                                 video_codec_ctx->width, video_codec_ctx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC,
                                 NULL, NULL, NULL);

        int rgb_picture_size = avpicture_get_size(AV_PIX_FMT_RGB24, video_codec_ctx->width, video_codec_ctx->height);
        rgb_frame_buffer = (uint8_t *) (av_malloc(rgb_picture_size * sizeof(uint8_t)));
        avpicture_fill((AVPicture *) video_RGB_frame, rgb_frame_buffer, AV_PIX_FMT_RGB24,
                       video_codec_ctx->width, video_codec_ctx->height);

        av_dump_format(av_fmt_ctx, 0, m_video_path, 0);

    }

}

FFmpegCapturer::~FFmpegCapturer() {

}

void FFmpegCapturer::start() {

}

bool FFmpegCapturer::captureFrame() {

    int ret;
    ret = av_read_frame(av_fmt_ctx, packet);

    if (ret < 0) {
        return false;
    }
    int got_picture;
    ret = avcodec_decode_video2(video_codec_ctx, video_frame, &got_picture, packet);
    if (ret < 0) {
        printf("decode video failed");
    }

    if (got_picture) {

    }


}


