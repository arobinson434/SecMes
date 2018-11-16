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
        std::string        mPeerKey;
        std::string        mPeerFile;
        int                mLocalBid;
        int                mPeerBid;

        bool waitForMsg(std::string&, int);
        bool getPeerInfo();
        bool waitForPeerName();
        bool waitForPeerKey();
        bool waitForPeerBid();
        bool setServerMode();
        void validateKey();
        bool isKnownPeerName();
        bool isValidPeerKey();
        void saveKey();

};

#endif
