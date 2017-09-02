#include "h264_decoder.h"

H264Decoder::H264Decoder() {

}

int H264Decoder::initDecoder() {
    pCodec = NULL;
    pCodecCtx = NULL;
    pVideFrame = NULL;
    pictureW = 0;
    pictureH = 0;

    av_register_all();
    avcodec_register_all();
    pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!pCodec) {

        printf("codec is  null!\n");
        return 0;

    }

    avcodec_open2(pCodecCtx, pCodec, NULL);
    pVideFrame = av_frame_alloc();

    return 1;

}

int H264Decoder::decodeH264Frames(unsigned char *inputBuffer, int length) {
    int gotPicPtr = 0;
    int result = 0;

    av_init_packet(&avPacket);
    avPacket.data = (unsigned char *) inputBuffer;
    avPacket.size = length;
    result = avcodec_decode_video2(pCodecCtx, pVideFrame, &gotPicPtr, &avPacket);

    if (pictureW != 0 && pictureW != pCodecCtx->width) {
        pictureW = pCodecCtx->width;
        return -1;
    }

    if (gotPicPtr) {

        unsigned int lumiLenght = (pCodecCtx->height) * (MIN(pVideFrame->linesize[0], pCodecCtx->width));
        unsigned int chromaBLength = (pCodecCtx->height / 2) * (MIN(pVideFrame->linesize[1], pCodecCtx->width / 2));
        unsigned int chromaRLength = (pCodecCtx->height / 2) * (MIN(pVideFrame->linesize[2], pCodecCtx->width / 2));
        H264Frame_YUV yuvFrame;

        memset(&yuvFrame, 0, sizeof(H264Frame_YUV));
        yuvFrame.luma.length = lumiLenght;
        yuvFrame.chromaB.length = chromaBLength;
        yuvFrame.chromaR.length = chromaRLength;

        yuvFrame.luma.dataBuffer = (unsigned char *) malloc(lumiLenght);
        yuvFrame.chromaB.dataBuffer = (unsigned char *) malloc(chromaBLength);
        yuvFrame.chromaR.dataBuffer = (unsigned char *) malloc(chromaRLength);
        copyDecodeFrame(pVideFrame->data[0], yuvFrame.luma.dataBuffer, pVideFrame->linesize[0],
                        pCodecCtx->width, pCodecCtx->height);
        copyDecodeFrame(pVideFrame->data[1], yuvFrame.chromaB.dataBuffer, pVideFrame->linesize[1],
                        pCodecCtx->width / 2, pCodecCtx->height / 2);
        copyDecodeFrame(pVideFrame->data[2], yuvFrame.chromaB.dataBuffer, pVideFrame->linesize[2],
                        pCodecCtx->width / 2, pCodecCtx->height / 2);

        yuvFrame.width = pCodecCtx->width;
        yuvFrame.height = pCodecCtx->height;

        free(yuvFrame.luma.dataBuffer);
        free(yuvFrame.chromaB.dataBuffer);
        free(yuvFrame.chromaR.dataBuffer);

    }


    av_free_packet(&avPacket);

    return 0;
}

void H264Decoder::releaseDecoder() {

    if (!pCodecCtx) {
        avcodec_close(pCodecCtx);
        pCodecCtx = NULL;
    }
    if (pVideFrame) {
        av_frame_free(&pVideFrame);
        pVideFrame = NULL;
    }

}

void H264Decoder::copyDecodeFrame(unsigned char *src, unsigned char *dist, int linesize, int width, int height) {
    width = MIN(linesize, width);
    for (int i = 0; i < height; ++i) {
        memcpy(dist, src, width);
        dist += width;
        src += linesize;
    }
}
