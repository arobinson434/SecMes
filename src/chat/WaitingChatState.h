#ifndef WAITING_CHAT_STATE_H
#define WAITING_CHAT_STATE_H

#include "chat/AbstractChatState.h"

class WaitingChatState: public AbstractChatState {
    public:
        WaitingChatState(ChatMachine*);
        ~WaitingChatState();

        AbstractChatState* run();
        std::string        stateHelp();
        bool               processStateCmd(std::string);

    private:
        AbstractChatState* mNextState;

        void connectLoop();
        void inputLoop();
};

#endif
