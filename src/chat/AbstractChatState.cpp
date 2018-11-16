#include <chrono>
#include <thread>
#include "ChatMachine.h"
#include "chat/AbstractChatState.h"

AbstractChatState::AbstractChatState(ChatMachine* machine) {
    mMachine = machine;
    mLogger  = Logger::getLogger();

    passwd* pw = getpwuid(getuid());
    mConfigDir  = std::string(pw->pw_dir) + "/.secmes";
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
        clearConvo();
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

void AbstractChatState::eventLoopSleep() {
    std::this_thread::sleep_for(std::chrono::milliseconds(EVENT_LOOP_SLEEP_MS));
}

std::string AbstractChatState::stateHelp() {
    return "";
}

bool AbstractChatState::processStateCmd(std::string cmd) {
    return false;
}

// Start of pass through functions
void AbstractChatState::setName(std::string name) {
    mMachine->mName = name;
}

std::string AbstractChatState::getName() {
    return mMachine->mName;
}

void AbstractChatState::setPeerName(std::string name) {
    mMachine->mPeerName = name;
}

std::string AbstractChatState::getPeerName() {
    return mMachine->mPeerName;
}

void AbstractChatState::setRunning(bool val) {
    mMachine->mRunning = val;
}

bool AbstractChatState::isRunning() {
    return mMachine->mRunning;
}

bool AbstractChatState::hasPendingMsg() {
    return mMachine->mNetEng->hasPendingMsg();
}

bool AbstractChatState::isConnected() {
    return mMachine->mNetEng->isConnected();
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

void AbstractChatState::disconnect() {
    mMachine->mNetEng->disconnect();
}

void AbstractChatState::initializeNetEngine(std::string port) {
    mMachine->mNetEng->initialize(port);
}

void AbstractChatState::initializeSecEngine(std::string secretKey) {
    mMachine->mSecEng->initialize(secretKey);
}

void AbstractChatState::setPeerPublicKey(std::string peerKey) {
    mMachine->mSecEng->setPeerPublicKey(peerKey);
}

void AbstractChatState::setIsServer(bool isServer) {
    mMachine->mSecEng->setIsServer(isServer);
}

void AbstractChatState::cleanSecEngine() {
    mMachine->mSecEng->clean();
}

std::string AbstractChatState::getPublicKey() {
    return mMachine->mSecEng->getPublicKey();
}

std::string AbstractChatState::generateSecretKey() {
    return mMachine->mSecEng->generateSecretKey();
}

std::string AbstractChatState::decryptMsg(std::string cipherText) {
    return mMachine->mSecEng->decryptMsg(cipherText);
}

std::string AbstractChatState::encryptMsg(std::string msg) {
    return mMachine->mSecEng->encryptMsg(msg);
}

bool AbstractChatState::hasPendingInput() {
    return mMachine->mChatWin->hasPendingInput();
}

std::string AbstractChatState::getUserInput() {
    return mMachine->mChatWin->getUserInput();
}

void AbstractChatState::writeToConvo(std::string msg) {
    mMachine->mChatWin->writeToConvo(msg);
}

void AbstractChatState::clearConvo() {
    mMachine->mChatWin->clearConvo();
}

