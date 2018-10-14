#ifndef WAITING_CHAT_STATE_H
#define WAITING_CHAT_STATE_H

#include "chat/AbstractChatState.h"

class WaitingChatState: public AbstractChatState {
    public:
        WaitingChatState(ChatMachine*);

        AbstractChatState* run();
        std::string        stateHelp();
        bool               processStateCmd(std::string);

    private:
        AbstractChatState* mNextState;
};

#endif
