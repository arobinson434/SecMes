#include "MockSecEngine.h"

MockSecEngine::MockSecEngine() {
    mLogger = Logger::getLogger();
}

void MockSecEngine::log(std::string msg) {
    mLogger->log("MockSecEngine: "+msg);
}

void MockSecEngine::initialize(std::string secretKey) {
    log("Loading the secret key");
}

void MockSecEngine::setPeerPublicKey(std::string) {
    log("Processing peer public key");
}

void MockSecEngine::setIsServer(bool isServer) {
    if ( isServer )
        log("Operating in server mode");
    else
        log("Operating in client mode");
}

void MockSecEngine::clean() {
    log("Clearing engine data cache");
}

std::string MockSecEngine::getPublicKey() {
    return "MyPublicKey";
}

std::string MockSecEngine::generateSecretKey() {
    return "AFakeAndVeryObviouslyInsecureKey";
}

std::string MockSecEngine::decryptMsg(std::string cipher_text) {
    return cipher_text; // We aren't actually encrypting or decrypting
}

std::string MockSecEngine::encryptMsg(std::string msg) {
    return msg; // We aren't actually encrypting or decrypting
}
