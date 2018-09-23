#ifndef ABS_CHAT_STATE_H
#define ABS_CHAT_STATE_H

#include <string>

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
        virtual std::string stateHelp();
        virtual bool        processStateCmd(std::string);

        // Machine Accessor
        void        setName(std::string);
        std::string getName();
        void        setRunning(bool);
        bool        isRunning();

        // NetEngine Accessors
        std::string getMsg();
        int         sendMsg(std::string);
        bool        connectRemote(std::string, std::string);
        void        closeRemote();
        void        initialize(std::string);
        bool        isConnected();

        // Window Accessors
        std::string readFromInput();
        void        writeToConvo(std::string);
        void        clear();
};

#endif
