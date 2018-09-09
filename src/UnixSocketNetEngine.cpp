#include "impl/UnixSocketNetEngine.h"

UnixSocketNetEngine::UnixSocketNetEngine(int port) {}

UnixSocketNetEngine::~UnixSocketNetEngine() {}

std::string UnixSocketNetEngine::getMsg() {
    return "stub";
}

int UnixSocketNetEngine::sendMsg(std::string msg) {
    return 0;
}

void* UnixSocketNetEngine::get_addr_ptr(sockaddr) {
        return (void*)this;
}

bool UnixSocketNetEngine::create_listeners() {
        return false;
}
