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
        bool        hasPendingMsg();
        std::string getMsg();
        int         sendMsg(std::string);
        bool        connectRemote(std::string, std::string);
        void        disconnect();
        bool        isConnected();
        void        initialize(std::string); 

    private:
        bool mIsConnected;
};

#endif
