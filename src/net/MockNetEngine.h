#ifndef MOCK_NET_ENGINE_H
#define MOCK_NET_ENGINE_H

#include <unistd.h>
#include "NetEngine.h"

/* This is just a testing stub.
 *  It will answer getMsg() with a delay, and stock response.
 *  It will return immediately from sendMsg()
 */
class MockNetEngine: public NetEngine {
    public:
        MockNetEngine();
        std::string getMsg();
        std::string processConnection();
        int         sendMsg(std::string);
        bool        connectRemote(std::string, std::string);
        void        closeRemote();
        void        initialize(std::string);
        bool        isConnected();

    private:
        bool mIsConnected;
};

#endif
