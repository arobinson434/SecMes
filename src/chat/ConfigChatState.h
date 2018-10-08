#ifndef CONFIG_CHAT_STATE_H
#define CONFIG_CHAT_STATE_H

#include "chat/AbstractChatState.h"

class ChatMachine;

class ConfigChatState: public AbstractChatState {
    public:
        ConfigChatState(ChatMachine*);
        AbstractChatState* run();
};

#endif 
