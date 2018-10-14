#include "chat/ConfigChatState.h"
#include "chat/WaitingChatState.h"

ConfigChatState::ConfigChatState(ChatMachine* machine):
    AbstractChatState(machine) {
    mModeName = "ConfigState";
}

AbstractChatState* ConfigChatState::run() {
    // TODO: This should be where we:
    //  1. Look for a config file on the system
    //  2. If it exists
    //      a.) parse it and move on
    //  3. If it doesn't exist,
    //      a.) get info from the user
    //      b.) write it to the config
    //      c.) then move on
    // Hard code for now
    log("Configuring fixed settings");
    setName("userA");
    initialize("3000");
    return new WaitingChatState(mMachine);
}
