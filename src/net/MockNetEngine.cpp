#include "net/MockNetEngine.h"

std::string MockNetEngine::processConnection() {
    return ""; // We don't spoof incoming connections
}

std::string MockNetEngine::getMsg() {
    sleep(5);
    mIsConnected = true;
    return std::string("userB: Test message!\n");
}

int MockNetEngine::sendMsg(std::string msg) {
    return 0;
}

bool MockNetEngine::connectRemote(std::string ip, std::string port) {
    mIsConnected = true;
    return true;
}

void MockNetEngine::closeRemote() {
    mIsConnected = false;
}

void MockNetEngine::initialize(std::string port) {
}

bool MockNetEngine::isConnected() {
    return mIsConnected;
}
