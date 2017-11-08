//
// Created by 58boy on 2017/11/7.
//

#include "video_streamer.h"

bool State::changeState(Context *context, State *state) {
    context->changeState(state);
    return true;
}

void Context::changeState(State *state) {
    this->m_state = state;
}

void Context::doWork() {
    m_state->doWork(this);
}

void Context::doChangeState() {
    m_state->doChangeState(this);
}

/**
 * 构建状态机
 * @param context
 */
void PausedState::doChangeState(Context *context) {
    doWork(context);
    changeState(context, new StartedState());
}

void StartedState::doChangeState(Context *context) {
    doWork(context);
    int i;
    switch (i) {
        case 0:
            changeState(context, new StoppedState);
            break;
        case 1:
            changeState(context, new PausedState());
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            changeState(context, new StartedState());
            break;
    }
}

void StoppedState::doChangeState(Context *context) {
    doWork(context);
    changeState(context, new PreparedState());

}

void PreparedState::doChangeState(Context *context) {
    doWork(context);
    changeState(context, new StartedState());
}

void IdleState::doChangeState(Context *context) {
    doWork(context);
    changeState(context, new InitializedState());
}

void InitializedState::doChangeState(Context *context) {
    doWork(context);
    changeState(context, new PreparedState());
}

/**
 * 状态实现
 * @param context
 */
void PreparedState::doWork(Context *context) {

}

void StartedState::doWork(Context *context) {

}

void PausedState::doWork(Context *context) {

}

void StoppedState::doWork(Context *context) {

}

void IdleState::doWork(Context *context) {
    printf("IdleState::doWork\n");
}

void InitializedState::doWork(Context *context) {

}
