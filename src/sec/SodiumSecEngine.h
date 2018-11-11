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
    
        Logger*       mLogger;
        bool          mIsServer;
        unsigned char mSecretKey[crypto_box_SECRETKEYBYTES];
        unsigned char mPublicKey[crypto_box_PUBLICKEYBYTES];
        unsigned char mPeerKey  [crypto_box_PUBLICKEYBYTES];
        unsigned char mRxNonce  [crypto_secretbox_NONCEBYTES];
        unsigned char mRxKey    [crypto_generichash_BYTES];
        unsigned char mTxNonce  [crypto_secretbox_NONCEBYTES];
        unsigned char mTxKey    [crypto_generichash_BYTES];
};

#endif
