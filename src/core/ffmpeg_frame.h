//
// Created by 58boy on 2017/9/22.
//

#ifndef FFMPEGSTUDIO_FFMPEG_FRAME_H
#define FFMPEGSTUDIO_FFMPEG_FRAME_H

extern "C" {
#include <libavutil/avutil.h>
};

class FFImage {
public:
    int width;
    int height;
    int width_step;
    unsigned char *image_data;//字节数组
    int image_size;
    int n_channels;
    int pix_type;
    int image_orientation;
    bool is_image_square;

    FFImage() {
        image_orientation = 0;
        is_image_square = false;
    }

    FFImage(int w, int h, int pix_t, int channels) {
        width = w;
        height = h;
        pix_type = pix_t;
        n_channels = channels;
        width_step = width * n_channels;
        image_size = width * width_step;
        image_data = 0;
        image_orientation = 0;
        is_image_square = false;
    }

    ~FFImage() {
        if (image_data != nullptr) {
            delete image_data;
            image_data = nullptr;
        }
    }

    static inline FFImage *createHeader(int w, int h, int pix_t, int channels) {
        return new FFImage(w, h, pix_t, channels);
    }
};

class Frame {
public:
    bool key_fame;
    long timestamp;
    long start_timestamp;
    FFImage *image;

    /**
     * audio
     */
//samples
    int nb_samples;
    int sample_rate;
    int sample_format;
    int sample_channels;
    long size;

    Frame() {
        key_fame = false;
        image = 0;
        nb_samples = 0;
        timestamp = 0;
        start_timestamp = 0;
        size = 0;
        sample_rate = 0;
        sample_format = -1;
        sample_channels = 0;
    }


};

typedef unsigned char BYTE;

class FFrame {

public:
    ~FFrame() {

        if (data != nullptr) {
            av_free(data);
        }

    }

public:
    BYTE *data;
    bool hasVideo = false;
    bool hasAudio = false;
    int length;
    int width;
    int height;
    double pts;

    int nChannels;
    int sampleRate;


};


#endif //FFMPEGSTUDIO_FFMPEG_FRAME_H
