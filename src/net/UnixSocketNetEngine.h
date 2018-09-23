#ifndef UNIX_SOCKET_NET_ENGINE_H
#define UNIX_SOCKET_NET_ENGINE_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "log/Logger.h"
#include "NetEngine.h"

class UnixSocketNetEngine: public NetEngine {
    public:
        UnixSocketNetEngine();
        ~UnixSocketNetEngine();

        std::string getMsg();
        std::string processConnection();
        int         sendMsg(std::string);
        bool        connectRemote(std::string, std::string);
        void        closeRemote();
        void        initialize(std::string);
        bool        isConnected();

    private:
        void* getAddrPtr(sockaddr*);
        bool  createListeners();
        void  log(std::string);

        fd_set      mListenFDs;
        int         mListenMax;
        int         mRemoteFD;
        bool        mIsConnected;
        std::string mPort;
        Logger*     mLogger;
        timeval     mTv;
};

#endif
