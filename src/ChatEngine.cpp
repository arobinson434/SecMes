#include <string>
#include <thread>
#include "ChatEngine.h"
#include "ChatWindow.h"
#include "SecEngine.h"
#include "NetEngine.h"

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
    me = "userA";

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
    while (mRunning) {
        inputData = mChatWin->readFromInput(); //blocking
        if (processCmd(inputData))
            continue;
        inputData = "me: " + inputData + "\n";
        if (mSecEng)
            inputData = mSecEng->encryptMsg(inputData);
        mNetEng->sendMsg(inputData);
        mChatWin->writeToConvo(inputData);
    }
}

bool ChatEngine::processCmd(std::string cmd) {
    if (cmd == "/quit") {
        stop();
        mChatWin->writeToConvo("System: Closing down...");
        return true;
    }
    else if (cmd == "/help") {
        std::string output = "";
        output += "System: Current Commands\n";
        output += "\thelp\tThis help text\n";
        output += "\tquit\tClose the app\n";
        mChatWin->writeToConvo(output);
        return true;
    }
    return false;
}

