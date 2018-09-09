#include "ChatEngine.h"
#include "impl/NcursesChatWindow.h"
#include "impl/MockNetEngine.h"
#include "impl/UnixSocketNetEngine.h"
#include "log/Logger.h"

int main() {
    Logger*           logger = Logger::getLogger();
    logger->log("Main: Starting up...");

    NcursesChatWindow chatWin;
    //MockNetEngine     netEngj
    UnixSocketNetEngine netEng("3000");

    ChatEngine        chatEng(&chatWin, NULL, &netEng);

    chatEng.run();

    logger->log("Main: Exiting...");
    return 0;
}
