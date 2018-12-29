#include <cstring>
#include "net/Message.h"

std::hash<std::string> hash;

Logger* Message::logger = Logger::getLogger();

void Message::log(std::string msg) {
    logger->log("MessageHandler: "+msg);
}

std::string Message::pack(std::string msg) {
    size_t packedMsgLen = msg.size() + (2 * sizeof(size_t));
    size_t msgHash      = hash(msg);
    char*  newMsgBuff   = new char[packedMsgLen]();

    int offset = 0;
    memcpy(newMsgBuff,        &packedMsgLen, sizeof(size_t));
    offset += sizeof(size_t);
    memcpy(newMsgBuff+offset, msg.c_str(),   msg.size());
    offset += msg.size();
    memcpy(newMsgBuff+offset, &msgHash,      sizeof(size_t));

    std::string retVal(newMsgBuff, packedMsgLen);
    delete newMsgBuff;

    return retVal;
}

std::string Message::unpack(std::string msg) {
    size_t packedMsgLen;
    size_t unpackedMsgLen = msg.size() - (2 * sizeof(size_t));
    size_t msgHash;

    int offset = 0;
    memcpy(&packedMsgLen, msg.c_str(), sizeof(size_t));
    offset += sizeof(size_t);
    if( msg.size() == packedMsgLen ) {
        std::string unpackedMsg(msg.c_str()+offset, unpackedMsgLen);
        offset += unpackedMsgLen;
        memcpy(&msgHash, msg.c_str()+offset, sizeof(size_t));
        if( msgHash == hash(unpackedMsg))
            return unpackedMsg;
        else
            log("Message hash did not match message!");
    }
    
    return std::string();
}

std::list<std::string> Message::unpackBuffer(char* buffer, size_t len) {
    std::list<std::string> retList;

    int   bytesToRead = len;
    int   offset      = 0;

    do {
        buffer += offset;
        size_t msgSize;
        memcpy(&msgSize, buffer, sizeof(size_t));

        if ( msgSize > (bytesToRead-offset) ) {
            log("Invalid message length! Dumping buffer.");
            break;
        }

        std::string msg(unpack(std::string(buffer, msgSize)));
        if( msg != "" )
            retList.push_back(msg);

        offset  += msgSize;
    } while ( offset < bytesToRead );

    return retList;
}

