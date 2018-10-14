#ifndef ABS_CHAT_STATE_H
#define ABS_CHAT_STATE_H

#include <sstream>
#include <string>

#define EVENT_LOOP_SLEEP_MS 100

class ChatMachine;
class Logger;

class AbstractChatState {
    public:
        AbstractChatState(ChatMachine*);
        ~AbstractChatState();
        virtual AbstractChatState* run() = 0;

    protected:
        std::string  mModeName;
        ChatMachine* mMachine;
        Logger*      mLogger;

        void                log(std::string);
        std::string         help();
        bool                processCmd(std::string);
        void                eventLoopSleep();
        virtual std::string stateHelp();
        virtual bool        processStateCmd(std::string);

        // Machine Accessor
        void        setName(std::string);
        std::string getName();
        void        setPeerName(std::string);
        std::string getPeerName();
        void        setRunning(bool);
        bool        isRunning();

        // NetEngine Accessors
        bool        hasPendingMsg();
        bool        isConnected();
        std::string getMsg();
        int         sendMsg(std::string);
        bool        connectRemote(std::string, std::string);
        void        disconnect();
        void        initialize(std::string);

        // Window Accessors
        bool        hasPendingInput();
        std::string getUserInput();
        void        writeToConvo(std::string);
        void        clearConvo();
};

#endif
