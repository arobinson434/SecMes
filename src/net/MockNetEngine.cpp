#include "net/MockNetEngine.h"

std::string MockNetEngine::getMsg() {
    sleep(5);
    return std::string("userB: Test message!\n");
}

int MockNetEngine::sendMsg(std::string msg) {
    return 0;
}

bool MockNetEngine::connectRemote(std::string ip, std::string port) {
    return true;
}

void MockNetEngine::closeRemote() {
}
