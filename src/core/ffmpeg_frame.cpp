//
// Created by 58boy on 2017/9/22.
//

#include "ffmpeg_frame.h"

FFrame::FFrame() {

}

void FFrame::copy(FFrame *frame) {

    this->hasAudio = frame->hasAudio;
    this->hasVideo = frame->hasVideo;
    this->length = frame->length;
    this->width = frame->width;
    this->height = frame->height;
    this->pts = frame->pts;
    this->nChannels = frame->nChannels;
    this->sampleRate = frame->sampleRate;
    this->channel_layout = frame->channel_layout;
    this->data = static_cast<BYTE *>(malloc(frame->length * sizeof(BYTE)));
    memcpy(this->data, frame->data, frame->length);
}
