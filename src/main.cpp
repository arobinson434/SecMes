#include "ChatMachine.h"
#include "window/NcursesChatWindow.h"
#include "net/UnixSocketNetEngine.h"
#include "sec/SodiumSecEngine.h"
#include "log/Logger.h"

int main() {
    Logger* logger = Logger::getLogger();
    logger->log("Main: Starting up...");

    NcursesChatWindow   chatWin;
    UnixSocketNetEngine netEng;
    SodiumSecEngine     secEng;
    ChatMachine         chatMach(&chatWin, &secEng, &netEng);

    chatMach.run();

    logger->log("Main: Exiting...");
    return 0;
}
