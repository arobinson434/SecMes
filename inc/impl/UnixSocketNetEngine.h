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

#include "NetEngine.h"

class UnixSocketNetEngine: public NetEngine {
    public:
        UnixSocketNetEngine(int port);
        ~UnixSocketNetEngine();

        std::string getMsg();
        int         sendMsg(std::string);

    private:
        void* get_addr_ptr(sockaddr);
        bool  create_listeners();

        fd_set  m_master_fds;
        int     m_fdmas;
        int     m_listenfd;
        int     m_remotefd;
};

#endif
