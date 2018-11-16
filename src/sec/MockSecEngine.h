#ifndef MOCK_SEC_ENGINE_H
#define MOCK_SEC_ENGINE_H

#include "SecEngine.h"
#include "log/Logger.h"

class MockSecEngine: public SecEngine {
    public:
        MockSecEngine();

        void        initialize(std::string);
        void        setPeerPublicKey(std::string);
        void        setIsServer(bool);
        void        clean();
        std::string getPublicKey();
        std::string generateSecretKey();
        std::string decryptMsg(std::string);
        std::string encryptMsg(std::string);

    private:
        Logger* mLogger;

        void log(std::string);    
};

#endif
