#ifndef CONFIG_CHAT_STATE_H
#define CONFIG_CHAT_STATE_H

#include "chat/AbstractChatState.h"

class ChatMachine;

class ConfigChatState: public AbstractChatState {
    public:
        ConfigChatState(ChatMachine*);
        AbstractChatState* run();

    private:
        std::string configDir;
        std::string configFile;

        std::string getInfo();
        bool        hasConfigDir();
        bool        hasConfig();
        bool        createConfigDir();
        void        createConfig();
        void        loadConfig();
};

#endif 
