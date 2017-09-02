#ifndef H264_DECODER_H
#define H264_DECODER_H

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#include "h264_frame.h"

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef ABS
#define ABS(a)  ((a)>0?(a):(-a))
#endif


class H264Decoder {
private:
    int pictureW;
    int pictureH;
    AVCodec *pCodec;
    AVCodecContext *pCodecCtx;
    AVFrame *pVideFrame;

    AVPacket avPacket;

public:
    H264Decoder();

    int initDecoder();

    void releaseDecoder();

    int decodeH264Frames(unsigned char *inputBuffer, int length);

    void copyDecodeFrame(unsigned char *src, unsigned char *dist, int linesize, int width, int height);

};

#endif // H264_DECODER_H
