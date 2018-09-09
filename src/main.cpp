
#include "ChatEngine.h"
#include "impl/NcursesChatWindow.h"
#include "impl/MockNetEngine.h"
#include "log/Logger.h"

int main() {
    Logger*           logger = Logger::getLogger();
    logger->log("Main: Starting up...");

    NcursesChatWindow chatWin;
    MockNetEngine     netEng;

    ChatEngine        chatEng(&chatWin, NULL, &netEng);

    chatEng.run();

    logger->log("Main: Exiting...");
    return 0;
}
