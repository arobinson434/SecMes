#include <ncurses.h>
#include <string>
#include "impl/NcursesChatWindow.h"

#define MAX_MSG_SIZE 1024

NcursesChatWindow::NcursesChatWindow() {
    mLogger = Logger::getLogger();
    mLogger->log("ChatWin: Initializing NcursesChatWindow...");
    
    initscr();
    cbreak();
    refresh();

    mConvoBox = createWindow(LINES-5,   COLS,       0, 0, true);
    mInputBox = createWindow(      5,   COLS, LINES-5, 0, true);
    mConvo    = createWindow(LINES-7, COLS-2,       1, 1, false);
    mInput    = createWindow(      3, COLS-2, LINES-4, 1, false);          

    scrollok(mConvo, TRUE);
}

NcursesChatWindow::~NcursesChatWindow() {
    mLogger->log("ChatWin: Tearing Down NcursesChatWindow...");
    endwin();
}

void NcursesChatWindow::writeToConvo(std::string msg) {
    wprintw(mConvo, msg.c_str());
    wrefresh(mConvo);
}

std::string NcursesChatWindow::readFromInput() {
    char input[MAX_MSG_SIZE];

    wclear(mInput);
    wgetstr(mInput, input);

    return std::string(input);
}

WINDOW* NcursesChatWindow::createWindow(int height, int width, int starty,
                                        int startx, bool draw_box) {
    WINDOW* retVal = newwin(height, width, starty, startx);
    if (draw_box)
        box(retVal, 0, 0);
    wrefresh(retVal);
    return retVal;
}
