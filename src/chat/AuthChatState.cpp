#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "chat/AuthChatState.h"
#include "chat/ChattingChatState.h"
#include "chat/WaitingChatState.h"

AuthChatState::AuthChatState(ChatMachine* machine):
    AbstractChatState(machine) {
    mModeName  = "AuthState";
    mNextState = 0;
    mWaitingOnResponse = true;

#ifdef TEST
    srand(10000);
#else
    srand(time(NULL));
#endif
}

AbstractChatState* AuthChatState::run() {
    mLocalBid = rand();

    if (getPeerInfo()) {
        validateKey();
        writeToConvo("\tPlease '/accept' or '/reject' the session.\n");
        while (isConnected() && isRunning() && mWaitingOnResponse) {
            if ( hasPendingInput() ) {
                std::string msg = getUserInput();
                processCmd(msg);
            }
            eventLoopSleep();
        }
    }

    if ( mWaitingOnResponse ) {
        log("Going back to waiting");
        writeToConvo("System: Connection closed\n");
        mNextState =  new WaitingChatState(mMachine);
    }

    return mNextState;
}

bool AuthChatState::waitForMsg(std::string& msg, int timeoutSec) {
    int attempts = timeoutSec * 10; // Once every 100ms for timeoutSec
    while (attempts > 0) {
        attempts--;
        if ( hasPendingMsg() ) {
            msg = getMsg();
            return true;
        }
        usleep(100 * 1000);
    }
    return false;
}

bool AuthChatState::getPeerInfo() {
    log("Fetching peer info...");
    log("Sending local username");
    sendMsg(getName());
    if (waitForPeerName()) {
        log("Got peer username!");
        log("Sending local public key");
        sendMsg(getPublicKey());
        if (waitForPeerKey()) {
            log("Got peer public key!");
            log("Sending bid");
            sendMsg(std::to_string(mLocalBid));
            if (waitForPeerBid()) {
                log("Got peer bid!");
                if (setServerMode()) {
                    return true;
                }
            }
        }
    }
    log("Failed to get peer info");
    return false;
}

bool AuthChatState::waitForPeerName() {
    std::string peerName;
    bool result = waitForMsg(peerName, 2);
    if ( result ) {
        setPeerName(peerName);
        mPeerFile = mConfigDir + "/" + peerName;
    }
    return result;
}

bool AuthChatState::waitForPeerKey() {
    bool result = waitForMsg(mPeerKey, 2);
    if ( result )
        setPeerPublicKey(mPeerKey);
    return result;
}

bool AuthChatState::waitForPeerBid() {
    std::string peerBidStr;
    bool result = waitForMsg(peerBidStr, 2);
    if ( result ) {
        mPeerBid = std::stoi(peerBidStr);
        result   = setServerMode();
    }
    return result;
}

bool AuthChatState::setServerMode() {
    if ( mLocalBid == mPeerBid ) {
        writeToConvo("System: Rotten Luck! Both sides entered identical bids.\n");
        log("Rotten Luck! Both sides entered identical bids.");
        return false;
    }

    if ( mLocalBid > mPeerBid )
        setIsServer(true);
    else
        setIsServer(false);

    return true;
}

void AuthChatState::validateKey() {
    if ( isKnownPeerName() ) {
        log("Found existing peer file for "+getPeerName());
        if ( isValidPeerKey() ){
            log("Connected to known user: " + getPeerName());
            writeToConvo("System: Connected to known user: " + getPeerName() + "\n");
        } else {
            log("WARNING! Connected to user " + getPeerName() +\
                " BUT public key does not match the local record!");
            writeToConvo("System: WARNING! Connected to user " + getPeerName() +\
                         " BUT public key does not match the local record!\n");
            writeToConvo("\tNote: Typing '/accept' will overwrite the existing\n");
            writeToConvo("\t\tkey with this new public key.\n");
        }
    } else {
        log("Connected to unknown user: " + getPeerName());
        writeToConvo("System: Connected to unknown user: " + getPeerName() + "\n");
    }
}

bool AuthChatState::isKnownPeerName() {
    struct stat fileInfo;

    if ( stat(mPeerFile.c_str(), &fileInfo) == 0 && \
         (fileInfo.st_mode & S_IFREG) )
        return true;

    return false;
}

bool AuthChatState::isValidPeerKey() {
    std::string   knownKey;
    std::ifstream keyStream;

    keyStream.open(mPeerFile);
    keyStream >> knownKey;
    keyStream.close();

    if ( knownKey == mPeerKey )
        return true;
    return false;
}

void AuthChatState::saveKey() {
    log("Saving peer public key");
    std::fstream keyStream;

    keyStream.open(mPeerFile, std::fstream::out | std::fstream::trunc);
    keyStream << mPeerKey;
    keyStream.close();
}

std::string AuthChatState::stateHelp() {
    std::string helpStr;
    helpStr += "\tAuth State Commands:\n";
    helpStr += "\t\t* accept - proceed with the remote connection\n";
    helpStr += "\t\t* reject - drop the remote connection\n";
    helpStr += "\n";
    return helpStr;
}

bool AuthChatState::processStateCmd(std::string cmd) {
    if (cmd == "/accept") {
        log("Chat session accepted\n");
        writeToConvo("System: Chat session accepted\n");
        if ( !( isKnownPeerName() && isValidPeerKey() ) )
            saveKey();
        mWaitingOnResponse = false;
        mNextState = new ChattingChatState(mMachine);
        return true;
    } else if (cmd == "/reject") {
        mWaitingOnResponse = false;
        mNextState = new WaitingChatState(mMachine);
        disconnect();
        log("Chat session rejected, connection closed\n");
        writeToConvo("System: Chat session rejected, connection closed\n");
        return true;
    }
    return false;
}

