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
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log/Logger.h"
#include "NetEngine.h"

class UnixSocketNetEngine: public NetEngine {
    public:
        UnixSocketNetEngine(std::string);
        ~UnixSocketNetEngine();

        std::string getMsg();
        int         sendMsg(std::string);
        bool        connectRemote(std::string, std::string);
        void        closeRemote();

    private:
        void* getAddrPtr(sockaddr*);
        bool  createListeners();
        void  log(std::string);

        fd_set      mMasterFDs;
        fd_set      mListenFDs;
        int         mFdmax;
        int         mRemoteFD;
        std::string mPort;
        Logger*     mLogger;
        bool        mAcceptConns;
};

#endif
