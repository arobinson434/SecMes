#ifndef SODIUM_SEC_ENGINE_H
#define SODIUM_SEC_ENGINE_H

#include <sodium.h>
#include "log/Logger.h"
#include "sec/SecEngine.h"

class SodiumSecEngine: public SecEngine {
    public:
        SodiumSecEngine();

        void        initialize(std::string);
        void        setPeerPublicKey(std::string);
        void        setIsServer(bool);
        void        clean();
        std::string getPublicKey();
        std::string generateSecretKey();
        std::string decryptMsg(std::string);
        std::string encryptMsg(std::string);

    private:
        void log(std::string);
    
        Logger* logger;
        bool    isServer;
        unsigned char    secretKey[crypto_box_SECRETKEYBYTES];
        unsigned char    publicKey[crypto_box_PUBLICKEYBYTES];
        unsigned char    peerKey  [crypto_box_PUBLICKEYBYTES];
        unsigned char    rxNonce  [crypto_secretbox_NONCEBYTES];
        unsigned char    rxKey    [crypto_generichash_BYTES];
        unsigned char    txNonce  [crypto_secretbox_NONCEBYTES];
        unsigned char    txKey    [crypto_generichash_BYTES];
};

#endif
