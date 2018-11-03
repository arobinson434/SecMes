#ifndef ABS_CHAT_STATE_H
#define ABS_CHAT_STATE_H

#include <pwd.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define EVENT_LOOP_SLEEP_MS 10

class ChatMachine;
class Logger;

class AbstractChatState {
    public:
        AbstractChatState(ChatMachine*);
        ~AbstractChatState();
        virtual AbstractChatState* run() = 0;

    protected:
        std::string  mConfigDir;
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
        void        initializeNetEngine(std::string);

        // SecEngine Accessors
        void        initializeSecEngine(std::string);
        void        setPeerPublicKey(std::string);
        void        setIsServer(bool);
        void        cleanSecEngine();
        std::string getPublicKey();
        std::string generateSecretKey();
        std::string decryptMsg(std::string);
        std::string encryptMsg(std::string);

        // Window Accessors
        bool        hasPendingInput();
        std::string getUserInput();
        void        writeToConvo(std::string);
        void        clearConvo();
};

#endif
