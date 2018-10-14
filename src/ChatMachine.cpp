#include "ChatMachine.h"
#include "chat/ConfigChatState.h"

ChatMachine::ChatMachine(ChatWindow* win, SecEngine* sec, NetEngine* net) {
    mState   = new ConfigChatState(this);
    mName    = "";
    mChatWin = win;
    mSecEng  = sec;
    mNetEng  = net;
    mLogger  = Logger::getLogger();
    mRunning = false;
}

ChatMachine::~ChatMachine() {
    if (mState)
        delete mState;
}

void ChatMachine::run() {
    mRunning = true;

    while ( mRunning && mState ) {
        AbstractChatState* currState = mState;
        mState = mState->run();
        delete currState;
    }
}
