//
// Created by 58boy on 2017/11/7.
//

#include "video_streamer.h"

using namespace Streamer;

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

State *Context::getState() {
    return m_state;
}

IStateChangeAction *Context::getAction() {
    return m_action;
}

/**
 * 构建状态机
 * @param context
 */
void PausedState::doChangeState(Context *context) {
    doWork(context);
    changeState(context, new StartedState(context));
}

void StartedState::doChangeState(Context *context) {

}

void StartedState::dispatchEvent(Context *context, Event event) {
    auto next = m_eventStateMap[event];
    event();
    changeState(context, next);
}


void StoppedState::doChangeState(Context *context) {
    doWork(context);
    changeState(context, new PreparedState());

}

void PreparedState::doChangeState(Context *context) {
    doWork(context);
    changeState(context, new StartedState(context));
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

StartedState::StartedState(Context *context) {
    m_eventStateMap.insert({context->getAction()->stop(), new StoppedState});
    m_eventStateMap.insert({context->getAction()->pause(), new PausedState});
    m_eventStateMap.insert({context->getAction()->seekTo(), new StartedState(context)});
    m_eventStateMap.insert({context->getAction()->start(), new StartedState(context)});
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

void StateMachine::transition(Context *context, Event event) {
    if (event == NULL) {
        context->getState()->doChangeState(context);
    } else {
        context->getState()->dispatchEvent(event);
    }
}
