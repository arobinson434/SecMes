#ifndef NET_ENGINE_H
#define NET_ENGINE_H

#include <string>

class NetEngine {
    public:
        virtual bool        hasPendingMsg() = 0;
        virtual std::string getMsg() = 0;
        virtual int         sendMsg(std::string) = 0;
        virtual bool        connectRemote(std::string, std::string) = 0;
        virtual void        disconnect() = 0;
        virtual bool        isConnected() = 0;
        virtual void        initialize(std::string) = 0;
};

#endif
