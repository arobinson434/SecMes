#include "ChatMachine.h"
#include "chat/AbstractChatState.h"

AbstractChatState::AbstractChatState(ChatMachine* machine) {
    mMachine = machine;
    mLogger  = Logger::getLogger();
}

AbstractChatState::~AbstractChatState() {}

std::string AbstractChatState::help() {
    std::string helpStr;
    helpStr += "System: Help Prompt\n";
    helpStr += "\tGlobal Commands:\n";
    helpStr += "\t\t* clear - Clear the screen\n";
    helpStr += "\t\t* help  - This help prompt\n";
    helpStr += "\t\t* quit  - Quit the program\n";
    helpStr += "\n";
    helpStr += stateHelp();
    return helpStr;
}

void AbstractChatState::log(std::string msg) {
    mLogger->log(mModeName + ": " + msg);
}

bool AbstractChatState::processCmd(std::string cmd) {
    if (cmd == "/clear") {
        clear();
        return true;
    }
    else if (cmd == "/help") {
        writeToConvo(help());
        return true;
    }
    else if (cmd == "/quit") {
        log("Quitting...");
        writeToConvo("System: Quitting...");
        setRunning(false);
        return true;
    }

    // If nothing hit here, pass on to the state's handler
    return processStateCmd(cmd);
}

void AbstractChatState::setName(std::string name) {
    mMachine->mName = name;
}

std::string AbstractChatState::stateHelp() {
    return "";
}

bool AbstractChatState::processStateCmd(std::string cmd) {
    return false;
}

// Start of pass through functions
std::string AbstractChatState::getName() {
    return mMachine->mName;
}

void AbstractChatState::setRunning(bool val) {
    mMachine->mRunning = val;
}

bool AbstractChatState::isRunning() {
    return mMachine->mRunning;
}

std::string AbstractChatState::getMsg() {
    return mMachine->mNetEng->getMsg();
}

int AbstractChatState::sendMsg(std::string msg) {
    return mMachine->mNetEng->sendMsg(msg);
}

bool AbstractChatState::connectRemote(std::string addr, std::string port) {
    return mMachine->mNetEng->connectRemote(addr, port);
}

void AbstractChatState::closeRemote() {
    mMachine->mNetEng->closeRemote();
}

void AbstractChatState::initialize(std::string port) {
    mMachine->mNetEng->initialize(port);
}

std::string AbstractChatState::readFromInput() {
    return mMachine->mChatWin->readFromInput();
}

void AbstractChatState::writeToConvo(std::string msg) {
    mMachine->mChatWin->writeToConvo(msg);
}

void AbstractChatState::clear() {
    mMachine->mChatWin->clear();
}

