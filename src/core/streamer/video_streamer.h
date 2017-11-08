//
// Created by 58boy on 2017/11/7.
//

#ifndef FFMPEGSTUDIO_VIDEO_STREAMER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

};
#define FFMPEGSTUDIO_VIDEO_STREAMER_H

class Context;

/**
 * interface
 */
class State {
public:
    virtual void doWork(Context *context);
};

class Context {
public:
    Context(State *state) {

    }

    ~Context() {
        delete m_state;
    }

public:
    void doWork() {
        m_state->doWork(this);
    }

    void setState(State *state) {
        m_state = state;
    }

private:
    State *m_state;
};

/**
 * 状态实现
 */

class IdleState : public State {
public:
    void doWork(Context *context) override;
};

class InitializedState : public State {
public:
    void doWork(Context *context) override;
};

class PreparedState : public State {
public:
    void doWork(Context *context) override;
};

class StartedState : public State {
public:
    void doWork(Context *context) override;
};

class PausedState : public State {
public:
    void doWork(Context *context) override;
};

class StoppedState : public State {
public:
    void doWork(Context *context) override;
};


class VideoStreamer {
public:
    VideoStreamer(char *path);

    void setOutput(char *path);

    void prepare();

    void start();


public:


};


#endif //FFMPEGSTUDIO_VIDEO_STREAMER_H
