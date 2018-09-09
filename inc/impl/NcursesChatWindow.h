#ifndef NCURSES_CHAT_WINDOW_H
#define NCURSES_CHAT_WINDOW_H

#include <ncurses.h>
#include <string>
#include "ChatWindow.h"
#include "log/Logger.h"

class NcursesChatWindow: public ChatWindow {
    public:
       NcursesChatWindow();
       ~NcursesChatWindow();

       void          writeToConvo(std::string);
       std::string   readFromInput();

    private:
       WINDOW* createWindow(int, int, int, int, bool);

       WINDOW* mConvoBox;
       WINDOW* mInputBox;
       WINDOW* mConvo;
       WINDOW* mInput;
       Logger* mLogger;
};

#endif
