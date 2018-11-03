#include "ChatMachine.h"
#include "window/NcursesChatWindow.h"
#include "net/UnixSocketNetEngine.h"
#include "sec/MockSecEngine.h"
#include "log/Logger.h"

int main() {
    Logger* logger = Logger::getLogger();
    logger->log("Main: Starting up...");

    NcursesChatWindow   chatWin;
    UnixSocketNetEngine netEng;
    MockSecEngine       secEng;
    ChatMachine         chatMach(&chatWin, &secEng, &netEng);

    chatMach.run();

    logger->log("Main: Exiting...");
    return 0;
}
