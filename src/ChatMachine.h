#ifndef CHAT_MACHINE_H
#define CHAT_MACHINE_H

#include <string>
#include "window/ChatWindow.h"
#include "sec/SecEngine.h"
#include "net/NetEngine.h"
#include "log/Logger.h"

class AbstractChatState;

class ChatMachine {
    friend AbstractChatState;
    public:
        ChatMachine(ChatWindow*, SecEngine*, NetEngine*);
        ~ChatMachine();

        void run();
    
    private:
        AbstractChatState* mState;
        std::string        mName;
        std::string        mPeerName;
        ChatWindow*        mChatWin;
        SecEngine*         mSecEng;
        NetEngine*         mNetEng;
        Logger*            mLogger;
        bool               mRunning;
};

#endif 
