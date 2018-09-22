#ifndef CHAT_ENGINE_H
#define CHAT_ENGINE_H

#include <string>
#include "window/ChatWindow.h"
#include "sec/SecEngine.h"
#include "net/NetEngine.h"
#include "log/Logger.h"

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
        Logger*     mLogger;
};

#endif
