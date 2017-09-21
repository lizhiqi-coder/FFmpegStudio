//
// Created by 58boy on 2017/9/16.
//

#include "VideoPlayer.h"


VideoPlayer::VideoPlayer(std::string path) {
    video_path = path;
}

void VideoPlayer::run() {

}


VideoPlayer::~VideoPlayer() {
    av_free(out_buffer);
    av_free(video_RGB_frame);
    avcodec_close(avCodecCtx);
    avformat_close_input(&avfmtCtx);

}

int VideoPlayer::init() {
    int ret;
    av_register_all();
    avfmtCtx = avformat_alloc_context();
//    if (video_path == NULL) {
//        return -1;
//    }
    ret = avformat_open_input(&avfmtCtx, video_path.c_str(), NULL, NULL);
    if (ret < 0) {
        printf("open input failed");
        return -1;
    }
    for (int i = 0; i < avfmtCtx->nb_streams; ++i) {
        if (avfmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        }
        if (avfmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
        }
    }
    if (video_stream_index == -1) {
        printf("did not find video_stream_index");
        return -1;
    }
    avCodecCtx = avfmtCtx->streams[video_stream_index]->codec;
    videoCodec = avcodec_find_decoder(avCodecCtx->codec_id);
    if (videoCodec == NULL) {
        printf("can not find avcodec");
        return -1;
    }
    if (avcodec_open2(avCodecCtx, videoCodec, NULL) < 0) {
        printf("open codec failed");
        return -1;
    }

    imageWidth = avCodecCtx->width;
    imageHeight = avCodecCtx->height;
    image_size = imageHeight * imageWidth;
    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket));
    av_new_packet(packet, image_size);

    audio_frame = av_frame_alloc();

    video_frame = av_frame_alloc();

    video_RGB_frame = av_frame_alloc();
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, avCodecCtx->width, avCodecCtx->height);
    out_buffer = (uint8_t *) (av_malloc(numBytes * sizeof(uint8_t)));

    avpicture_fill((AVPicture *) video_RGB_frame, out_buffer,
                   AV_PIX_FMT_RGB24, avCodecCtx->width, avCodecCtx->height);

    image_convert_ctx = sws_getContext(avCodecCtx->width, avCodecCtx->height,
                                       avCodecCtx->pix_fmt, avCodecCtx->width, avCodecCtx->height,
                                       AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

    av_dump_format(avfmtCtx, 0, video_path.c_str(), 0);




//    if (audio_stream_index == -1) {
//
//    } else {
//        printf("did not find video_stream_index");
//
//    }

}

void VideoPlayer::grabFrame() {
    int ret;
    int got_picture;
    ret = av_read_frame(avfmtCtx, packet);
    if (ret < 0) {
        return;
    }

    if (packet->stream_index == video_stream_index) {
        //decode
        ret = avcodec_decode_video2(avCodecCtx, video_frame, &got_picture, packet);
        if (ret < 0) {
            printf("decode video frame failed");
            return;
        }
        //convert YUV to RGB
        if (got_picture) {
            sws_scale(image_convert_ctx, (const uint8_t *const *) video_frame->data,
                      video_frame->linesize, 0, avCodecCtx->height,
                      video_RGB_frame->data, video_RGB_frame->linesize);

            saveFrame(video_RGB_frame, avCodecCtx->width, avCodecCtx->height);
        }
    }

}

void VideoPlayer::saveFrame(AVFrame *frame, int w, int h) {
    FILE *pfile;
    std::string file_path;
    pfile = fopen(file_path.c_str(), "wb");
    if (pfile == NULL) {
        return;
    }
    for (int i = 0; i < h; ++i) {
        fwrite(frame->data[0] + i * frame->linesize[0], 1, w * 3, pfile);
    }

    fclose(pfile);
}
