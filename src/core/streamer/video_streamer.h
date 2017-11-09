//
// Created by 58boy on 2017/11/7.
//

#ifndef FFMPEGSTUDIO_VIDEO_STREAMER_H
#define FFMPEGSTUDIO_VIDEO_STREAMER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

};

#include <functional>
#include <map>

using Event = std::function<void()>;
namespace Streamer {
    class Context;

/**
 * interface
 */
    class State {

    public:
        virtual void doWork(Context *context)=0;

        virtual void doChangeState(Context *context)=0;

        virtual void dispatchEvent(std::function<void()> event);

    protected:
        std::map<Event, State*> m_eventStateMap;
        bool changeState(Context *context, State *state);

    };

    class IStateChangeAction {

    public:
        virtual void setDataSource(char *path)=0;

        virtual void prepare()=0;

        virtual void seekTo()=0;

        virtual void start()=0;

        virtual void pause()=0;

        virtual void stop()=0;

        virtual void reset()=0;

        virtual void release()=0;

    };

    class Context {

    public:
        Context(State *state, IStateChangeAction *action) {
            m_state = state;
            m_action = action;
        }

        ~Context() {
            delete m_state;
        }

    public:
        void doWork();

        void doChangeState();

        void changeState(State *state);

        State *getState();

        IStateChangeAction *getAction();

    private:
        State *m_state;
        IStateChangeAction *m_action;
    };


    /**
     * 状态机，负责调度管理
     */
    class StateMachine {

    public:
        void transition(Context *context, std::function<void()> eventImp);
    };

/**
 * 状态实现
 */

    class IdleState : public State {
    public:
        virtual void doWork(Context *context);

        virtual void doChangeState(Context *context);
    };

    class InitializedState : public State {
    public:
        virtual void doWork(Context *context);

        virtual void doChangeState(Context *context);

    };

    class PreparedState : public State {
    public:
        virtual void doWork(Context *context);

        virtual void doChangeState(Context *context);

    };

    class StartedState : public State {
    public:
        StartedState(Context *context);

    public:
        virtual void doWork(Context *context);

        virtual void doChangeState(Context *context);

        void dispatchEvent(Context*context,Event event);

    };

    class PausedState : public State {
    public:
        virtual void doWork(Context *context);

        virtual void doChangeState(Context *context);

    };

    class StoppedState : public State {
    public:
        virtual void doWork(Context *context);

        virtual void doChangeState(Context *context);

    };


    /**
     * 推流器实现
     */
    class VideoStreamer : public IStateChangeAction {

    };
}


#endif //FFMPEGSTUDIO_VIDEO_STREAMER_H
