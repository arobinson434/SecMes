#include "net/UnixSocketNetEngine.h"
#include <cstring>
#include <sstream>

#define BACKLOG 10
#define MAX_BUF 1024

UnixSocketNetEngine::UnixSocketNetEngine() {
    FD_ZERO(&mListenFDs);

    mListenMax   = mRemoteFD = 0;
    mLogger      = Logger::getLogger();
    mIsConnected = false;
    
}

UnixSocketNetEngine::~UnixSocketNetEngine() {
    mRunning = false;
    mRcvThread->join();

    disconnect();
    for ( int fd = 0; fd < mListenMax; fd++ ) {
        if ( FD_ISSET(fd, &mListenFDs) ) {
            close(fd);
        }
    }
}

void UnixSocketNetEngine::initialize(std::string port) {
    mPort = port;
    log("Starting the Unix Socket Net Engine...");
    if (createListeners()) {
        log("Successfully started Listener");
        mRunning   = true;
        mRcvThread = new std::thread(&UnixSocketNetEngine::rcvLoop, this);
    } else {
        log("FAILED TO START LISTENER!");
    }
}

void UnixSocketNetEngine::rcvLoop() {
    while (mRunning) {
        if (mIsConnected) {
            rcvMsg();
        } else {
            rcvConnection();
        }

        // We won't yield until we 'select' again, and at that point 
        //  we will have the mutex. Lets yield to see if any other threads
        //  need the mutex.
        std::this_thread::yield();
    }
}

void UnixSocketNetEngine::rcvConnection() {
    int              tmpFd;
    char             ipstr[INET6_ADDRSTRLEN];
    sockaddr_storage remoteAddr;
    socklen_t        addrSize  = sizeof remoteAddr;
    fd_set           readFds   = mListenFDs;
    timeval          tv;

    tv.tv_sec  = 0;
    tv.tv_usec = 10 * 1000; //10ms

    if ( select(mListenMax, &readFds, NULL, NULL, &tv) == -1 ) {
        log("Failed select in rcvConn()");
        perror("RcvCon Select");
    }

    for ( int fd = 0; fd < mListenMax; fd++ ) {
        if ( FD_ISSET(fd, &readFds) ) {
            tmpFd = accept(fd, (sockaddr*)&remoteAddr, &addrSize);
            if (tmpFd == -1) {
                log("Problem accepting connection!");
                perror("Accept");
                continue;
            }

            inet_ntop(remoteAddr.ss_family,
                      getAddrPtr((sockaddr *)&remoteAddr),
                      ipstr,
                      sizeof ipstr);

            mRemoteFD    = tmpFd;
            mIsConnected = true;
            tv.tv_sec    = 0;
            tv.tv_usec   = 10 * 1000; //10ms
            setsockopt(mRemoteFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

            log("Accepted Connection from " + std::string(ipstr));
            break;
        }
    }
}

void UnixSocketNetEngine::rcvMsg() {
    mRemoteFdMutex.lock();

    fd_set  remoteSet;
    char    buff[MAX_BUF];
    timeval tv;

    tv.tv_sec  = 0;
    tv.tv_usec = 10 * 1000; //10ms
    memset(buff, 0, MAX_BUF);
    FD_ZERO(&remoteSet);
    FD_SET(mRemoteFD, &remoteSet);

    if ( select(mRemoteFD+1, &remoteSet, NULL, NULL, &tv) == -1 ) {
        log("Failed select in rcvMsg()");
        perror("RcvMsg Select");
    }

    if ( FD_ISSET(mRemoteFD, &remoteSet) ) {
        int recBytes = recv(mRemoteFD, buff, MAX_BUF, 0); 
        if (recBytes == 0) {
            log("Remote end disconnected");
            disconnect();
        } else {
            mRcvQueue.push(std::string(buff));
        }
    }

    mRemoteFdMutex.unlock();
}

int UnixSocketNetEngine::sendMsg(std::string msg) {
    int sentBytes = 0;

    sentBytes = send(mRemoteFD, msg.c_str(), msg.length(), 0);
    if (sentBytes == -1) {
        log("Failed to send message");
        perror("Send");
    }

    return sentBytes;
}

bool UnixSocketNetEngine::connectRemote(std::string ip, std::string port) {
    int sockfd, rv;
    addrinfo hints, *remote, *p;

    memset(&hints, 0, sizeof  hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ( getaddrinfo(ip.c_str(), port.c_str(), &hints, &remote) != 0 ) {
        log("Error getting address info for "+ip);
        log(std::string("\tgetaddrinfo error: ")+gai_strerror(rv));
        return false;
    }

    for (p = remote; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if ( sockfd == -1 ) {
            log("Failed to get socket");
            perror("Socket");
            continue;
        }
        
        if ( connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) {
            log("Failed to connect to "+ip);
            perror("Connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        log("Failed to connect to "+ip);
        return false;
    }

    freeaddrinfo(remote);

    mRemoteFD    = sockfd;
    mIsConnected = true;

    timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = 10 * 1000; //10ms
    setsockopt(mRemoteFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    log("Successfully connected to "+ip);

    return true;
}

void UnixSocketNetEngine::disconnect() {
    mRemoteFdMutex.lock();

    log("Disconnecting");
    if (mRemoteFD)
        close(mRemoteFD);
    mIsConnected = false;
    mRemoteFD    = 0;

    mRemoteFdMutex.unlock();
}

bool UnixSocketNetEngine::hasPendingMsg() {
    return !(mRcvQueue.size() == 0);
}

std::string UnixSocketNetEngine::getMsg() {
    std::string retVal = mRcvQueue.front();
    mRcvQueue.pop();
    return retVal;
}

void* UnixSocketNetEngine::getAddrPtr(sockaddr* saddr) {
    if( saddr->sa_family == AF_INET )
        return &(((sockaddr_in*)saddr)->sin_addr);
    return &(((sockaddr_in6*)saddr)->sin6_addr);
}

bool UnixSocketNetEngine::createListeners() {
    addrinfo  hints, *res, *p;
    char      ipstr[INET6_ADDRSTRLEN];
    int       sockfd, rv;

    memset(&hints, 0, sizeof hints); // Zero out our struct
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags    = AI_PASSIVE;  // Serves on all interfaces

    rv = getaddrinfo(NULL, mPort.c_str(), &hints, &res);
    if ( rv != 0 ) {
        log("Failed getaddrinfo!");
        log("Error: " + std::string(gai_strerror(rv)));
        return false;
    }

    for ( p = res; p != NULL; p = p->ai_next ) {
        inet_ntop(p->ai_family, getAddrPtr(p->ai_addr), ipstr, sizeof ipstr);
        log("Found IP - " + std::string(ipstr));

        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if ( sockfd == -1 ) {
            log("\tFailed to get socket!");
            perror("\t\tSocket");
            continue;
        }

        int yes = 1;
        // Don't let AF_INET6 sockets dual stack!
        if ( p->ai_family == AF_INET6 ) {
            log("\tSetting sock opt!");
            setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(yes));
        }
    
        // Always bind!
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        if ( bind(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) {
            close(sockfd);
            log("\tFailed bind to socket!");
            perror("\t\tBind");
            continue;
        }

        if ( listen(sockfd, BACKLOG) == -1 ){
            log("\tFailed listen on socket!");
            perror("\t\tError");
            continue;
        }

        log("Listening on: " + std::string(ipstr));

        FD_SET(sockfd, &mListenFDs);
        mListenMax = sockfd+1;
    }

    freeaddrinfo(res);

    if ( mListenMax == 0 ) {
        log("Failed listen on any socket!");
        return false;
    }

    return true;
}

void UnixSocketNetEngine::log(std::string msg) {
    mLogger->log("UnixSockNetEng: "+msg);
}

bool UnixSocketNetEngine::isConnected() {
    return mIsConnected;
}
