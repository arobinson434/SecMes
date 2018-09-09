#ifndef NET_ENGINE_H
#define NET_ENGINE_H

#include <string>

class NetEngine {
    public:
        /* Note to the Implementor:
         *  You MUST NOT block infinitely in getMsg().I would suggest not
         *  waiting for message for longer than 1-2 seconds. I no message is
         *  received, return "". 
         */
        virtual std::string  getMsg() = 0;
        virtual int          sendMsg(std::string) = 0;
        virtual bool         connectRemote(std::string, std::string) = 0;
        virtual void         closeRemote() = 0; //Close the remote connection
};

#endif
