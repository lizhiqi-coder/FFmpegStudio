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
    doActiveEvent(context);
    changeState(context, new StartedState(context));
}

void StartedState::doChangeState(Context *context) {
    doActiveEvent(context);
    changeState(context, new EndState);
}

bool StartedState::dispatchEvent(Context *context, Event event) {
    if (event == NULL) {
        return false;
    }
    auto next = m_eventStateMap[event];
    if (next == NULL || next == nullptr) {
        return false;
    }
    event();
    changeState(context, next);
    return true;
}


void StoppedState::doChangeState(Context *context) {
    doActiveEvent(context);
    changeState(context, new PreparedState());

}

void PreparedState::doChangeState(Context *context) {
    doActiveEvent(context);
    changeState(context, new StartedState(context));
}

void IdleState::doChangeState(Context *context) {
    doActiveEvent(context);
    changeState(context, new InitializedState());
}

void InitializedState::doChangeState(Context *context) {
    doActiveEvent(context);
    changeState(context, new PreparedState());
}

/**
 * 状态实现
 * @param context
 */

StartedState::StartedState(Context *context) {
    m_eventStateMap.insert({context->getAction()->stop(), new StoppedState});
    m_eventStateMap.insert({context->getAction()->pause(), new PausedState});
    m_eventStateMap.insert({context->getAction()->seekTo(), new StartedState(context)});
    m_eventStateMap.insert({context->getAction()->start(), new StartedState(context)});
}

void StateMachine::transition(Context *context, Event event) {
    if (event == NULL) {
        context->getState()->doChangeState(context);
    } else {
        context->getState()->dispatchEvent(context, event);
    }
}

void StateMachine::transition(Context *context) {
    transition(context, NULL);
}
