
#include "ChatEngine.h"
#include "impl/NcursesChatWindow.h"
#include "impl/MockNetEngine.h"

int main() {
    NcursesChatWindow chatWin;
    MockNetEngine     netEng;

    ChatEngine        chatEng(&chatWin, NULL, &netEng);

    chatEng.run();

    return 0;
}
