#include "chat/AuthChatState.h"
#include "chat/WaitingChatState.h"

#include <unistd.h>

WaitingChatState::WaitingChatState(ChatMachine* machine):
    AbstractChatState(machine) {
    mModeName  = "WaitingState";
    mNextState = 0;
}

AbstractChatState* WaitingChatState::run() {
    log("Waiting for connection event");
    writeToConvo("System: Ready ( run \"/help\", to see available options)\n");

    while( !isConnected() && isRunning() ) {
        if ( hasPendingInput() ) {
            std::string msg = getUserInput();
            processCmd( msg );
        }
        eventLoopSleep();
    }

    if ( isConnected() ) {
        log("Connected!");
        mNextState = new AuthChatState(mMachine);
    }

    return mNextState;
}

std::string WaitingChatState::stateHelp() {
    std::string helpStr;
    helpStr += "\tWaiting State Commands:\n";
    helpStr += "\t\t* connect <ip> <port> - Connect to a remote host\n";
    helpStr += "\n";
    return helpStr;
}

bool WaitingChatState::processStateCmd(std::string cmd) {
    if (cmd.find("/connect") == 0) {
        std::stringstream ss(cmd);
        std::string ip, port;
        ss.ignore(10, ' ');
        ss >> ip >> port;
        
        if ( ip.size() == 0 || port.size() == 0 ) {
            writeToConvo("System: Failed to parse! Please specify the IP and Port\n");
        } else {
            log("Attempting to connect to "+ip+":"+port+" ...");
            writeToConvo("System: Attempting to connect to "+ip+":"+port+" ...\n");
            if (connectRemote(ip, port)) {
                log("Successfully connected");
                writeToConvo("System: Successfully connected\n");
            } else {
                log("Failed to connect");
                writeToConvo("System: Failed to connect!\n");
            }
        }
        return true;
    }
    return false;
}

