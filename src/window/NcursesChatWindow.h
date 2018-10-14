#ifndef NCURSES_CHAT_WINDOW_H
#define NCURSES_CHAT_WINDOW_H

#include <ncurses.h>
#include <queue>
#include <string>
#include <thread>

#include "ChatWindow.h"
#include "log/Logger.h"

class NcursesChatWindow: public ChatWindow {
    public:
       NcursesChatWindow();
       ~NcursesChatWindow();

       void          clearConvo();
       void          writeToConvo(std::string);
       bool          hasPendingInput();
       std::string   getUserInput();

    private:
       WINDOW* createWindow(int, int, int, int, bool);
       void    inputLoop();

       bool    mRunning;
       WINDOW* mConvoBox;
       WINDOW* mInputBox;
       WINDOW* mConvo;
       WINDOW* mInput;
       Logger* mLogger;

       std::queue<std::string> mInputQueue;
       std::thread*            mInputThread;
};

#endif
