#ifndef NET_ENGINE_H
#define NET_ENGINE_H

#include <string>

class NetEngine {
    public:
        //NetEngine();
        //~NetEngine();

        virtual std::string  getMsg() = 0;
        virtual int          sendMsg(std::string) = 0;
};

#endif
