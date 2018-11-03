#ifndef CONFIG_CHAT_STATE_H
#define CONFIG_CHAT_STATE_H

#include "chat/AbstractChatState.h"

class ChatMachine;

class ConfigChatState: public AbstractChatState {
    public:
        ConfigChatState(ChatMachine*);
        AbstractChatState* run();

    private:
        std::string configFile;
        std::string secKeyFile;

        std::string getInfo();
        bool        hasConfigDir();
        bool        hasConfig();
        bool        hasSecretKey();
        bool        createConfigDir();
        void        createConfig();
        void        createSecretKey();
        void        loadConfig();
};

#endif 
