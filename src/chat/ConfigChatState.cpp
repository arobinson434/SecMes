#include "chat/ConfigChatState.h"

ConfigChatState::ConfigChatState(ChatMachine* machine):
    AbstractChatState(machine) {
    mModeName = "ConfigState";
}

ConfigChatState::~ConfigChatState() {}

AbstractChatState* ConfigChatState::run() {
    // TODO: This should be where we:
    //  1. Look for a config file on the system
    //  2. If it exists
    //      a.) parse it and moe on
    //  3. If it doesn't exist,
    //      a.) get info from the user
    //      b.) write it to the config
    //      c.) then move on
    // Hard code for nowa
    log("Configuring fixed settings\n")
    setName("userA");
    initialize("3000");
    //return new WaitingChatState(mMachine);
    return new ConfigChatState(mMachine);
}
