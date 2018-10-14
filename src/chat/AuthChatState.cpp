#include <unistd.h>
#include "chat/AuthChatState.h"
#include "chat/ChattingChatState.h"
#include "chat/WaitingChatState.h"

AuthChatState::AuthChatState(ChatMachine* machine):
    AbstractChatState(machine) {
    mModeName  = "AuthState";
    mNextState = 0;
    mWaitingOnResponse = true;
}

AbstractChatState* AuthChatState::run() {
    // This stage is kinda dumb without the Security Engine.
    //  Once the SecEng is inplace, this code should:
    //  1.) Accept the remote user name, and public key.
    //  2.) If that public key is already known to be tied to that user,
    //      i.e. It is cached in our config dir, then continue to the chatting
    //      stage
    //  3.) If we have never heard of that user, ask the user to accept/reject,
    //      and either save the name and key, or close the connection.
    //  4.) If the key does not match the existing key for that user, warn
    //      the local user, and ask them to accept/reject the new key, and
    //      either save the updated key, or reject the connection accordingly.
    // 
    // For now, we will just have the user accept/reject any connection.

    sendMsg(getName());
    
    if ( getAuthInfo() ) {
        log("Connected to " + getPeerName());
        writeToConvo("System: Connected to " + getPeerName() + "\n");
        writeToConvo("\tPlease '/accept' or '/reject' the session.\n");
        while (isConnected() && isRunning()) {
            if ( hasPendingInput() ) {
                std::string msg = getUserInput();
                if ( processCmd( msg ) && !mWaitingOnResponse )
                    return mNextState;
            }
            eventLoopSleep();
        }
    }

    // The other end hungup or we quit
    log("Going back to waiting");
    writeToConvo("System: Connection closed\n");
    return new WaitingChatState(mMachine);
}

/*
 * Wait for auth info from the remote end
 */
bool AuthChatState::getAuthInfo() {
    int attempts = 20; // Once every 100ms for 2s
    while (attempts > 0) {
        attempts--;
        if ( hasPendingMsg() ) {
            setPeerName(getMsg());
            return true;
        }
        usleep(100 * 1000);
    }
    return false;
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

