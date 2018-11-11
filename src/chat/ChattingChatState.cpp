#include "chat/ChattingChatState.h"
#include "chat/WaitingChatState.h"

ChattingChatState::ChattingChatState(ChatMachine* machine):
    AbstractChatState(machine) {
    mModeName  = "ChattingState";
    mNextState = 0;
}

AbstractChatState* ChattingChatState::run() {
    while ( isConnected() && isRunning() ) {
        if ( hasPendingMsg() ) {
            writeToConvo(getPeerName()+": "+decryptMsg(getMsg())+"\n");   
        }
        if ( hasPendingInput() ) {
            std::string msg = getUserInput();
            if ( !processCmd(msg) ) {
                sendMsg(encryptMsg(msg));
                writeToConvo("me: "+msg+"\n");   
            }
        }
        eventLoopSleep();
    }

    log("Connection closed");
    writeToConvo("System: Connection closed\n");
    return new WaitingChatState(mMachine);
}

std::string ChattingChatState::stateHelp() {
    std::string helpStr;
    helpStr += "\tChatting State Commands:\n";
    helpStr += "\t\t* disconnect - end this conversation\n";
    helpStr += "\n";
    return helpStr;
}

bool ChattingChatState::processStateCmd(std::string cmd) {
    if ( cmd == "/disconnect" ) {
        disconnect();
        return true;
    }
    return false;
}


