#ifndef SEC_ENGINE_H
#define SEC_ENGINE_H

#include <string>

class SecEngine {
    public:
        SecEngine();
        ~SecEngine();

        virtual std::string decryptMsg(std::string) = 0;
        virtual std::string encryptMsg(std::string) = 0;
};

#endif
