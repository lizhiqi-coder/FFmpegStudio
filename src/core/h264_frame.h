#ifndef H264_FRAME_H
#define H264_FRAME_H


typedef struct H264Frame {
    unsigned int length;
    unsigned char *dataBuffer;
} H264Frame;
typedef struct H264Frame_YUV {

    unsigned int width;
    unsigned int height;
    H264Frame luma;
    H264Frame chromaB; //色彩的浓度、色度
    H264Frame chromaR;

} H264Frame_YUV;
#endif // H264_FRAME_H
