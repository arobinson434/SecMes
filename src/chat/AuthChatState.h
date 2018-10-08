#ifndef AUTH_CHAT_STATE_H
#define AUTH_CHAT_STATE_H

#include "chat/AbstractChatState.h"

class AuthChatState: public AbstractChatState {
    public:
        AuthChatState(ChatMachine*);

        AbstractChatState* run();
        std::string        stateHelp();
        bool               processStateCmd(std::string);

    private:
        AbstractChatState* mNextState;
        bool               mWaitingOnResponse;

        bool getAuthInfo();
};

#endif
