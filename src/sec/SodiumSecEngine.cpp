#include <cstring>
#include <iomanip>
#include <sstream>
#include "SodiumSecEngine.h"

#define MAX_MSG_SIZE 1024
#define CIPHER_TEXT_SIZE (crypto_secretbox_MACBYTES + MAX_MSG_SIZE)

// Debug helper
std::string hexStr(unsigned char* ar, size_t len) {
    std::stringstream ss;
    for (int i=0; i < len; ++i) {
        if (i%16 == 0)
            ss << std::endl;
        ss << " " << std::setfill('0') << std::setw(2) << std::hex << (int)ar[i];
    }
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
    std::memcpy(peerKey, _peerKey.c_str(), _peerKey.size());

    crypto_generichash_state hashState;
    unsigned char sharedKey[crypto_generichash_BYTES*2];
    unsigned char scalarmult[crypto_scalarmult_BYTES];

    if (crypto_scalarmult(scalarmult, secretKey, peerKey)) {
        log("Problem generating shared key!");
    } else {
        crypto_generichash_init  (&hashState, NULL, 0U,   sizeof sharedKey);
        crypto_generichash_update(&hashState, scalarmult, sizeof scalarmult);
        if (isServer) {
            crypto_generichash_update(&hashState, publicKey,  sizeof publicKey);
            crypto_generichash_update(&hashState, peerKey,    sizeof peerKey);
        } else {
            crypto_generichash_update(&hashState, peerKey,    sizeof peerKey);
            crypto_generichash_update(&hashState, publicKey,  sizeof publicKey);
        }
        crypto_generichash_final (&hashState, sharedKey,  sizeof sharedKey);
    }

    if (isServer) {
        std::memcpy(rxKey, sharedKey, crypto_generichash_BYTES);
        std::memcpy(txKey, sharedKey + crypto_generichash_BYTES, crypto_generichash_BYTES);
    } else {
        std::memcpy(txKey, sharedKey, crypto_generichash_BYTES);
        std::memcpy(rxKey, sharedKey + crypto_generichash_BYTES, crypto_generichash_BYTES);
    }

    log("set scalarmult to: "+hexStr(scalarmult, sizeof scalarmult));
    log("set shared key to: "+hexStr(sharedKey,  sizeof sharedKey));
    log("set rx key to: "+hexStr(rxKey,  sizeof rxKey));
    log("set tx key to: "+hexStr(txKey,  sizeof txKey));
    log(" TODO: Remove ^ !");
}

void SodiumSecEngine::setIsServer(bool _isServer) {
    isServer = _isServer;
}

void SodiumSecEngine::clean() {
    isServer = false;

    sodium_memzero(secretKey, sizeof secretKey);
    sodium_memzero(publicKey, sizeof publicKey);
    sodium_memzero(peerKey,   sizeof peerKey);
    sodium_memzero(rxNonce,   sizeof rxNonce);
    sodium_memzero(rxKey,     sizeof rxKey);
    sodium_memzero(txNonce,   sizeof txNonce);
    sodium_memzero(txKey,     sizeof txKey);
}

std::string SodiumSecEngine::getPublicKey() {
    return std::string(reinterpret_cast<char*>(publicKey), sizeof publicKey);
}

std::string SodiumSecEngine::generateSecretKey() {
    unsigned char tmpKey[crypto_box_SECRETKEYBYTES];
    randombytes_buf(tmpKey, sizeof tmpKey);
    return std::string(reinterpret_cast<char*>(tmpKey), sizeof tmpKey);
}

std::string SodiumSecEngine::decryptMsg(std::string _cipherText) {
    sodium_increment(rxNonce, sizeof rxNonce);
    
    unsigned char cipherText[CIPHER_TEXT_SIZE];
    unsigned char plainText[MAX_MSG_SIZE];

    std::memcpy(cipherText, _cipherText.c_str(), sizeof cipherText);
    if (crypto_secretbox_open_easy(plainText, cipherText,
                                   sizeof cipherText, rxNonce, rxKey) != 0) {
        log("Error detected, possibly forged message!!!");
    }

    return std::string(reinterpret_cast<char*>(plainText), std::strlen(reinterpret_cast<char*>(plainText)));
}

std::string SodiumSecEngine::encryptMsg(std::string msg) {
    sodium_increment(txNonce, sizeof txNonce);

    unsigned char plainText[MAX_MSG_SIZE];
    unsigned char cipherText[CIPHER_TEXT_SIZE];

    std::memcpy(plainText, msg.c_str(), sizeof plainText);
    crypto_secretbox_easy(cipherText, plainText,
                          sizeof plainText, txNonce, txKey);

    return std::string(reinterpret_cast<char*>(cipherText), sizeof cipherText);
}

void SodiumSecEngine::log(std::string msg) {
    logger->log("SodiumSecEng: "+msg);
}
