#ifndef MESSAGE_H
#define MESSAGE_H

#include <list>
#include <string>

#include "log/Logger.h"

class Message {
    public:
        static std::string pack(std::string msg);
        static std::string unpack(std::string msg);
        static std::list<std::string> unpackBuffer(char*, size_t);

    private:
        static Logger* logger;

        static void log(std::string);
};

#endif
