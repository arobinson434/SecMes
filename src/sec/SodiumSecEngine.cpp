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
    mLogger = Logger::getLogger();
    clean();

    if (sodium_init() < 0)
        log("Problem encountered initialising libsodium!!!");
}

void SodiumSecEngine::initialize(std::string secretKey) {
    std::memcpy(mSecretKey, secretKey.c_str(), sizeof mSecretKey);
    crypto_scalarmult_base(mPublicKey, mSecretKey);
}

void SodiumSecEngine::setPeerPublicKey(std::string peerKey) {
    std::memcpy(mPeerKey, peerKey.c_str(), peerKey.size());

    crypto_generichash_state hashState;
    unsigned char sharedKey[crypto_generichash_BYTES*2];
    unsigned char scalarmult[crypto_scalarmult_BYTES];

    if (crypto_scalarmult(scalarmult, mSecretKey, mPeerKey)) {
        log("Problem generating shared key!");
    } else {
        crypto_generichash_init  (&hashState, NULL, 0U,   sizeof sharedKey);
        crypto_generichash_update(&hashState, scalarmult, sizeof scalarmult);
        if (mIsServer) {
            crypto_generichash_update(&hashState, mPublicKey, sizeof mPublicKey);
            crypto_generichash_update(&hashState, mPeerKey,   sizeof mPeerKey);
        } else {
            crypto_generichash_update(&hashState, mPeerKey,   sizeof mPeerKey);
            crypto_generichash_update(&hashState, mPublicKey, sizeof mPublicKey);
        }
        crypto_generichash_final (&hashState, sharedKey, sizeof sharedKey);
    }

    if (mIsServer) {
        std::memcpy(mRxKey, sharedKey, crypto_generichash_BYTES);
        std::memcpy(mTxKey, sharedKey + crypto_generichash_BYTES, crypto_generichash_BYTES);
    } else {
        std::memcpy(mTxKey, sharedKey, crypto_generichash_BYTES);
        std::memcpy(mRxKey, sharedKey + crypto_generichash_BYTES, crypto_generichash_BYTES);
    }
}

void SodiumSecEngine::setIsServer(bool isServer) {
    mIsServer = isServer;
}

void SodiumSecEngine::clean() {
    mIsServer = false;

    sodium_memzero(mSecretKey, sizeof mSecretKey);
    sodium_memzero(mPublicKey, sizeof mPublicKey);
    sodium_memzero(mPeerKey,   sizeof mPeerKey);
    sodium_memzero(mRxNonce,   sizeof mRxNonce);
    sodium_memzero(mRxKey,     sizeof mRxKey);
    sodium_memzero(mTxNonce,   sizeof mTxNonce);
    sodium_memzero(mTxKey,     sizeof mTxKey);
}

std::string SodiumSecEngine::getPublicKey() {
    return std::string(reinterpret_cast<char*>(mPublicKey), sizeof mPublicKey);
}

std::string SodiumSecEngine::generateSecretKey() {
    unsigned char tmpKey[crypto_box_SECRETKEYBYTES];
    randombytes_buf(tmpKey, sizeof tmpKey);
    return std::string(reinterpret_cast<char*>(tmpKey), sizeof tmpKey);
}

std::string SodiumSecEngine::decryptMsg(std::string _cipherText) {
    sodium_increment(mRxNonce, sizeof mRxNonce);
    
    unsigned char cipherText[CIPHER_TEXT_SIZE];
    unsigned char plainText[MAX_MSG_SIZE];

    std::memcpy(cipherText, _cipherText.c_str(), sizeof cipherText);
    if (crypto_secretbox_open_easy(plainText, cipherText,
                                   sizeof cipherText, mRxNonce, mRxKey) != 0) {
        log("ERROR! Message corrupted!");
        return std::string("ERROR! Message corrupted!");
    }

    return std::string(reinterpret_cast<char*>(plainText), std::strlen(reinterpret_cast<char*>(plainText)));
}

std::string SodiumSecEngine::encryptMsg(std::string msg) {
    sodium_increment(mTxNonce, sizeof mTxNonce);

    unsigned char plainText[MAX_MSG_SIZE];
    unsigned char cipherText[CIPHER_TEXT_SIZE];

    std::memcpy(plainText, msg.c_str(), sizeof plainText);
    crypto_secretbox_easy(cipherText, plainText,
                          sizeof plainText, mTxNonce, mTxKey);

    return std::string(reinterpret_cast<char*>(cipherText), sizeof cipherText);
}

void SodiumSecEngine::log(std::string msg) {
    mLogger->log("SodiumSecEng: "+msg);
}
