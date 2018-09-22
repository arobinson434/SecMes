#include <sstream>
#include <string>
#include <thread>
#include "ChatEngine.h"
#include "window/ChatWindow.h"
#include "sec/SecEngine.h"
#include "net/NetEngine.h"

ChatEngine::ChatEngine(ChatWindow* _chatWin, SecEngine* _secEng, NetEngine* _netEng) {
    mRunning = false;
    mChatWin = _chatWin;
    mSecEng  = _secEng;
    mNetEng  = _netEng;
    mLogger  = Logger::getLogger();
}

ChatEngine::~ChatEngine() {
}

void ChatEngine::run() {
    mLogger->log("ChatEngine: Starting ChatEngine...");
    mRunning = true;
    me = "userA"; //TODO: This should be loaded from a config file

    std::thread recMsgThread (&ChatEngine::recMsgLoop, this);
    std::thread inputThread  (&ChatEngine::inputLoop, this);
    
    mLogger->log("ChatEngine: Running");

    inputThread.join();
    recMsgThread.join();

    mLogger->log("ChatEngine: Stopped");
}

void ChatEngine::stop() {
    mLogger->log("ChatEngine: Stopping...");
    mRunning = false;
}

void ChatEngine::recMsgLoop() {
    std::string recData;
    while (mRunning) {
        recData = mNetEng->getMsg(); // blocking
        if (recData.empty())
            continue;
        if (mSecEng)
            recData = mSecEng->decryptMsg(recData);
        mChatWin->writeToConvo(recData);
    }
}

void ChatEngine::inputLoop() {
    std::string inputData;
    int         sentBytes;
    while (mRunning) {
        inputData = mChatWin->readFromInput(); //blocking
        if (processCmd(inputData))
            continue;
        inputData = me + ": " + inputData + "\n";
        if (mSecEng)
            inputData = mSecEng->encryptMsg(inputData);
        sentBytes = mNetEng->sendMsg(inputData);
        if (sentBytes == inputData.length()) {
            mChatWin->writeToConvo(inputData);
        } else {
            mChatWin->writeToConvo("System: Failed to send message.\n");
        }
    }
}

//TODO: Move all of this out to different classes that register with the engine
bool ChatEngine::processCmd(std::string cmd) {
    if (cmd == "/quit") {
        stop();
        mChatWin->writeToConvo("System: Closing down...");
        return true;
    }
    else if (cmd == "/help") {
        std::string output = "";
        output += "System: Current Commands\n";
        output += "\tclear\tClear the screen\n";
        output += "\tconnect <ip> <port>\tConnect to another chat instance\n";
        output += "\tdisconnect\tClose the active connection\n";
        output += "\thelp\tThis help text\n";
        output += "\tquit\tClose the app\n";
        mChatWin->writeToConvo(output);
        return true;
    }
    else if (cmd.find("/connect") == 0) {
        std::stringstream ss(cmd);
        std::string ip, port;
        ss.ignore(10, ' ');
        ss >> ip >> port;
        
        if ( ip.size() == 0 || port.size() == 0 ) {
            mChatWin->writeToConvo("System: Failed to parse! please specify the IP and Port\n");
        } else {
            mChatWin->writeToConvo("System: Attempting to connect to "+ip+":"+port+" ...\n");
            if (mNetEng->connectRemote(ip, port)) {
                mChatWin->writeToConvo("System: Successfully connected\n");
            } else {
                mChatWin->writeToConvo("System: Failed to connected!\n");
            }
        }

        return true;
    }
    else if (cmd == "/clear") {
        mChatWin->clear();
        return true;
    }
    else if (cmd == "/disconnect") {
        mChatWin->writeToConvo("System: disconnected!\n");
        mNetEng->closeRemote();
        return true;
    }

    return false;
}

