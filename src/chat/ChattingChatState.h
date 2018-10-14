#ifndef CHATTING_CHAT_STATE_H
#define CHATTING_CHAT_STATE_H

#include "chat/AbstractChatState.h"

class ChattingChatState: public AbstractChatState {
    public:
        ChattingChatState(ChatMachine*);

        AbstractChatState* run();
        std::string        stateHelp();
        bool               processStateCmd(std::string);

    private:
        AbstractChatState* mNextState;
        std::string        remoteUser;

        bool getAuthInfo();
};

#endif
