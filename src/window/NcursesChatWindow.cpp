#include <ncurses.h>
#include <string>
#include "window/NcursesChatWindow.h"

#define MAX_MSG_SIZE 1024

NcursesChatWindow::NcursesChatWindow() {
    // TODO: Should this move into the start routine?
    mLogger = Logger::getLogger();
    mLogger->log("ChatWin: Initializing NcursesChatWindow...");
    
    initscr();
    cbreak();
    noecho();
    refresh();

    mConvoBox = createWindow(LINES-5,   COLS,       0, 0, true);
    mInputBox = createWindow(      5,   COLS, LINES-5, 0, true);
    mConvo    = createWindow(LINES-7, COLS-2,       1, 1, false);
    mInput    = createWindow(      3, COLS-2, LINES-4, 1, false);          

    wtimeout( mInput, 2000);

    scrollok(mConvo, TRUE);

    mRunning = true;
    mInputThread = new std::thread(&NcursesChatWindow::inputLoop, this);
}

NcursesChatWindow::~NcursesChatWindow() {
    mRunning = false;
    mInputThread->join();

    mLogger->log("ChatWin: Tearing Down NcursesChatWindow...");
    endwin();
}

void NcursesChatWindow::clearConvo() {
    wclear(mConvo);
    wrefresh(mConvo);
}

void NcursesChatWindow::writeToConvo(std::string msg) {
    wprintw(mConvo, msg.c_str());
    wrefresh(mConvo);
}

bool NcursesChatWindow::hasPendingInput() {
    return !(mInputQueue.size() == 0);
}

std::string NcursesChatWindow::getUserInput() {
    std::string retVal = mInputQueue.front();
    mInputQueue.pop();
    return retVal;
}

void NcursesChatWindow::inputLoop() {
    int         inputChar;
    std::string inputString;

    while (mRunning) {
        inputChar   = wgetch(mInput);
        
        if (inputChar != ERR) {
            if (inputChar == 127) { // backspace key
                int row,col;
                getyx(mInput, row, col);
                wmove(mInput, row, col-1);
                wdelch(mInput);
                wrefresh(mInput);
                inputString = inputString.substr(0, inputString.size()-1);
            } else if ( char(inputChar) == '\n' ) {
                wclear(mInput);
                wrefresh(mInput);
                mInputQueue.push(inputString);
                inputString = "";
            } else {
                wprintw(mInput, std::string(1, char(inputChar)).c_str());
                inputString += inputChar;
            }
        }
    }
}

WINDOW* NcursesChatWindow::createWindow(int height, int width, int starty,
                                        int startx, bool draw_box) {
    WINDOW* retVal = newwin(height, width, starty, startx);
    if (draw_box)
        box(retVal, 0, 0);
    wrefresh(retVal);
    return retVal;
}
