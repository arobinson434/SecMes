#ifndef CHAT_ENGINE_H
#define CHAT_ENGINE_H

#include <string>
#include "ChatWindow.h"
#include "SecEngine.h"
#include "NetEngine.h"

class ChatEngine {
    public:
        ChatEngine(ChatWindow*, SecEngine*, NetEngine*);
        ~ChatEngine();

        void run();
        void stop();

    private:
        void recMsgLoop();
        void inputLoop();
        bool processCmd(std::string);

        bool   mRunning;
        std::string me;

        ChatWindow* mChatWin;
        SecEngine*  mSecEng;
        NetEngine*  mNetEng;
};

#endif
