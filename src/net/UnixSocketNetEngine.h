#ifndef UNIX_SOCKET_NET_ENGINE_H
#define UNIX_SOCKET_NET_ENGINE_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <queue>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <mutex>
#include <thread>
#include <unistd.h>

#include "log/Logger.h"
#include "NetEngine.h"

class UnixSocketNetEngine: public NetEngine {
    public:
        UnixSocketNetEngine();
        ~UnixSocketNetEngine();

        bool        hasPendingMsg();
        std::string getMsg();
        int         sendMsg(std::string);
        bool        connectRemote(std::string, std::string);
        void        disconnect();
        bool        isConnected();
        void        initialize(std::string);

    private:
        void*       getAddrPtr(sockaddr*);
        bool        createListeners();
        void        log(std::string);
        void        rcvLoop();
        void        rcvConnection();
        void        rcvMsg();

        fd_set      mListenFDs;
        int         mListenMax;
        int         mRemoteFD;
        bool        mIsConnected;
        bool        mRunning;
        std::string mPort;
        Logger*     mLogger;
        timeval     mTv;

        std::thread*            mRcvThread;
        std::queue<std::string> mRcvQueue;
        std::recursive_mutex    mRemoteFdMutex;
};

#endif
