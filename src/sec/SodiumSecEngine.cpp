#include <cstring>
#include <iomanip>
#include <sstream>
#include "SodiumSecEngine.h"

std::string hexStr(unsigned char* ar, size_t len) {
    std::stringstream ss;
    for (int i=0; i < len; ++i)
        ss << std::setfill('0') << std::setw(2) << std::hex << (int)ar[i];
    return ss.str();
}

SodiumSecEngine::SodiumSecEngine() {
    logger = Logger::getLogger();
    clean();

    if (sodium_init() < 0)
        log("Problem encountered initialising libsodium!!!");
}

void SodiumSecEngine::initialize(std::string _secretKey) {
    std::memcpy(secretKey, _secretKey.c_str(), sizeof secretKey);
    crypto_scalarmult_base(publicKey, secretKey);
    
    log("set secret key to: "+hexStr(secretKey, sizeof secretKey));
    log("set public key to: "+hexStr(publicKey, sizeof publicKey));
    log(" TODO: Remove ^ !");
}

void SodiumSecEngine::setPeerPublicKey(std::string _peerKey) {
    std::memcpy(peerKey, _peerKey.c_str(), sizeof peerKey);

    crypto_generichash_state hashState;
    unsigned char scalarmult[crypto_scalarmult_BYTES];

    if (crypto_scalarmult(scalarmult, secretKey, peerKey)) {
        log("Problem generating shared key!");
    } else {
        crypto_generichash_init  (&hashState, NULL, 0U,   sizeof sharedKey);
        crypto_generichash_update(&hashState, scalarmult, sizeof scalarmult);
        crypto_generichash_update(&hashState, publicKey,  sizeof publicKey);
        crypto_generichash_update(&hashState, peerKey,    sizeof peerKey);
        crypto_generichash_final (&hashState, sharedKey,  sizeof sharedKey);
    }
}

void SodiumSecEngine::setIsServer(bool _isServer) {
    isServer = _isServer;
}

void SodiumSecEngine::clean() {
    isServer = false;
    rxNonce  = 0;
    txNonce  = 0;

    sodium_memzero(secretKey, sizeof secretKey);
    sodium_memzero(publicKey, sizeof publicKey);
    sodium_memzero(sharedKey, sizeof sharedKey);
    sodium_memzero(peerKey,   sizeof peerKey);
}

std::string SodiumSecEngine::getPublicKey() {
    return std::string(reinterpret_cast<char*>(publicKey), sizeof publicKey);
}

std::string SodiumSecEngine::generateSecretKey() {
    unsigned char tmpKey[crypto_box_SECRETKEYBYTES];
    randombytes_buf(tmpKey, sizeof tmpKey);
    return std::string(reinterpret_cast<char*>(tmpKey), sizeof tmpKey);
}

std::string SodiumSecEngine::decryptMsg(std::string cipherText) {
    return cipherText; //stub
}

std::string SodiumSecEngine::encryptMsg(std::string msg) {
    return msg; //stub 
}

void SodiumSecEngine::log(std::string msg) {
    logger->log("SodiumSecEng: "+msg+"\n");
}
