#include "ChatMachine.h"
#include "window/NcursesChatWindow.h"
#include "net/UnixSocketNetEngine.h"
#include "net/MockNetEngine.h"
#include "log/Logger.h"

int main() {
    Logger* logger = Logger::getLogger();
    logger->log("Main: Starting up...");

    NcursesChatWindow   chatWin;
    //UnixSocketNetEngine netEng;
    MockNetEngine netEng;
    ChatMachine         chatMach(&chatWin, NULL, &netEng);

    chatMach.run();

    logger->log("Main: Exiting...");
    return 0;
}
