#include <string.h>
#include <sys/time.h>
#include "net/UnixSocketNetEngine.h"

#define BACKLOG 10
#define MAX_BUF 1024

UnixSocketNetEngine::UnixSocketNetEngine(std::string port) {
    FD_ZERO(&mMasterFDs);
    FD_ZERO(&mListenFDs);

    mFdmax       = mRemoteFD = 0;
    mLogger      = Logger::getLogger();
    mPort        = port;
    mAcceptConns = true;

    log("Starting the Unix Socket Net Engine...");
    if (createListeners())
        log("Successfully started Listener");
    else
        log("FAILED TO START LISTENER!");
}

UnixSocketNetEngine::~UnixSocketNetEngine() {
    closeRemote();
    for ( int fd = 0; fd < mFdmax; fd++ ) {
        if ( FD_ISSET(fd, &mMasterFDs) ) {
            close(fd);
        }
    }
}

std::string UnixSocketNetEngine::getMsg() {
    char             ipstr[INET6_ADDRSTRLEN];
    char             buff[MAX_BUF];
    sockaddr_storage remoteAddr;
    socklen_t        addrSize  = sizeof remoteAddr;
    fd_set           readFds = mMasterFDs;
    std::string      retStr   = "";
    int              tmpFd;
    timeval          tv;
    
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    memset(buff, 0, MAX_BUF);
    
    if ( select(mFdmax, &readFds, NULL, NULL, &tv) == -1 ) {
        log("Failed select!");
        perror("Select");
        return retStr;
    }

    for ( int fd = 0; fd < mFdmax; fd++ ) {
        if ( FD_ISSET(fd, &readFds) ) {
            if ( FD_ISSET(fd, &mListenFDs) ) {
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

                if (mAcceptConns) {
                    mRemoteFD = tmpFd;
                    log("Accepted Connection from " + std::string(ipstr));
                    retStr += "System: Accepted Connection from " + std::string(ipstr) + "\n";
                    FD_SET(mRemoteFD, &mMasterFDs);
                    mFdmax = mRemoteFD + 1;
                } else {
                    close(tmpFd);
                    log("Rejected Connection from " + std::string(ipstr));
                }
            } else {
                // Process incoming messages
                int recBytes = recv(fd, buff, MAX_BUF, 0); 
               
                if (recBytes < 0) {
                    log("Error processing receive!\n");
                    perror("recv");
                } else if (recBytes == 0) {
                    log("Remote end disconnected\n");
                    retStr += "System: Remote end disconnected\n";
                    closeRemote();
                } else {
                    // We got something!
                    retStr += std::string(buff);
                }
            }
        }
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
    mRemoteFD       = sockfd;
    mFdmax          = mRemoteFD + 1;
    mAcceptConns    = false;
    FD_SET(mRemoteFD, &mMasterFDs);

    log("Successfully connected to "+ip+"\n");

    return true;
}

void UnixSocketNetEngine::closeRemote() {
    close(mRemoteFD);
    FD_CLR(mRemoteFD, &mMasterFDs);
    mAcceptConns = true;
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

        FD_SET(sockfd, &mMasterFDs);
        FD_SET(sockfd, &mListenFDs);
        mFdmax = sockfd+1;
    }

    freeaddrinfo(res);

    if ( mFdmax == 0 ) {
        log("Failed listen on any socket!");
        return false;
    }

    return true;
}

void UnixSocketNetEngine::log(std::string msg) {
    mLogger->log("UnixSockNetEng: "+msg);
}
