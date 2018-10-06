#include "chat/WaitingChatState.h"

WaitingChatState::WaitingChatState(ChatMachine* machine):
    AbstractChatState(machine) {
    mModeName  = "WaitingState";
    mNextState = 0;
}

AbstractChatState* WaitingChatState::run() {
    log("Waiting for connection instructions\n");

    //TODO: don't leave this busy wait in
    while( !isConnected() ) {
        if ( hasPendingInput() ) {
            std::string msg = getUserInput();
            processCmd( msg );
        }
    }

    return mNextState;
}

std::string WaitingChatState::stateHelp() {
    std::string helpStr;
    helpStr += "\tWaiting State Commands:\n";
    helpStr += "\t\t* clear - Clear the screen\n";
    helpStr += "\t\t* help  - This help prompt\n";
    helpStr += "\t\t* quit  - Quit the program\n";
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
            writeToConvo("System: Failed to parse! please specify the IP and Port\n");
        } else {
            writeToConvo("System: Attempting to connect to "+ip+":"+port+" ...\n");
            if (connectRemote(ip, port)) {
                writeToConvo("System: Successfully connected\n");
            } else {
                writeToConvo("System: Failed to connected!\n");
            }
        }
        return true;
    }
    return false;
}

