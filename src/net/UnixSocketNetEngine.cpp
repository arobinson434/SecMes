#include "net/UnixSocketNetEngine.h"
#include <cstring>

#define BACKLOG 10
#define MAX_BUF 1024

UnixSocketNetEngine::UnixSocketNetEngine() {
    FD_ZERO(&mListenFDs);

    mListenMax   = mRemoteFD = 0;
    mLogger      = Logger::getLogger();
    mIsConnected = false;
    
    mTv.tv_sec = 0;
    mTv.tv_usec = 500 * 1000; // 500ms
}

UnixSocketNetEngine::~UnixSocketNetEngine() {
    closeRemote();
    for ( int fd = 0; fd < mListenMax; fd++ ) {
        if ( FD_ISSET(fd, &mListenFDs) ) {
            close(fd);
        }
    }
}

void UnixSocketNetEngine::initialize(std::string port) {
    mPort = port;
    log("Starting the Unix Socket Net Engine...");
    if (createListeners())
        log("Successfully started Listener");
    else
        log("FAILED TO START LISTENER!");
}

std::string UnixSocketNetEngine::processConnection() {
    int              tmpFd;
    char             ipstr[INET6_ADDRSTRLEN];
    sockaddr_storage remoteAddr;
    socklen_t        addrSize  = sizeof remoteAddr;
    fd_set           readFds   = mListenFDs;
    std::string      retStr    = "";
    
    if ( mIsConnected ) {
        log("Currently connected, won't process new connections!");
        return retStr;
    }

    if ( select(mListenMax, &readFds, NULL, NULL, &mTv) == -1 ) {
        log("Failed select!");
        perror("Select");
        return retStr;
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
            setsockopt(mRemoteFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&mTv, sizeof mTv);

            log("Accepted Connection from " + std::string(ipstr));
            retStr += "System: Accepted Connection from " + std::string(ipstr) + "\n";
            break;
        }
    }

    return retStr;
}

std::string UnixSocketNetEngine::getMsg() {
    std::string retStr = "";
    char        buff[MAX_BUF];
    memset(buff, 0, MAX_BUF);

    if ( !mIsConnected )
        return retStr;

    int recBytes = recv(mRemoteFD, buff, MAX_BUF, 0); 
   
    if ( recBytes < 0 ) {
        if ( !( errno == EAGAIN || errno == EWOULDBLOCK ) ) {
            log("Error processing receive!\n");
            perror("recv");
        } // Else, we just didn't get anything
    } else if (recBytes == 0) {
        log("Remote end disconnected\n");
        retStr += "System: Remote end disconnected\n";
        closeRemote();
    } else {
        retStr += std::string(buff);
    }

    return retStr;
}

int UnixSocketNetEngine::sendMsg(std::string msg) {
    int sentBytes = 0;

    sentBytes = send(mRemoteFD, msg.c_str(), msg.length(), 0);
    if (sentBytes == -1) {
        log("Failed to send message\n");
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
        log("Error getting address info for "+ip+"!\n");
        log(std::string("\tgetaddrinfo error: ")+gai_strerror(rv)+"\n");
        return false;
    }

    for (p = remote; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if ( sockfd == -1 ) {
            log("Failed to get socket\n");
            perror("Socket");
            continue;
        }
        
        if ( connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) {
            log("Failed to connect to "+ip+"\n");
            perror("Connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        log("Failed to connect to "+ip+"\n");
        return false;
    }

    freeaddrinfo(remote);

    mRemoteFD    = sockfd;
    mIsConnected = true;
    setsockopt(mRemoteFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&mTv, sizeof mTv);

    log("Successfully connected to "+ip+"\n");

    return true;
}

void UnixSocketNetEngine::closeRemote() {
    close(mRemoteFD);
    mIsConnected = true;
    mRemoteFD    = 0;
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

        char yes = 1;
        // Don't let AF_INET6 sockets dual stack!
        if ( p->ai_family == AF_INET6 ) {
            setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(char));
        }
    
        // Always bind!
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(char));

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
