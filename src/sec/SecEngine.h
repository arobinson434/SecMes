#ifndef SEC_ENGINE_H
#define SEC_ENGINE_H

#include <string>

class SecEngine {
    public:
        virtual void        initialize(std::string)       = 0;
        virtual void        setPeerPublicKey(std::string) = 0;
        virtual void        setIsServer(bool)             = 0;
        virtual void        clean()                       = 0;
        virtual std::string getPublicKey()                = 0;
        virtual std::string generateSecretKey()           = 0;
        virtual std::string decryptMsg(std::string)       = 0;
        virtual std::string encryptMsg(std::string)       = 0;
};

#endif
