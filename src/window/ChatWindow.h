#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <string>

/*
 * A Generic ChatWindow, regardless of implementation, should be made up of
 *  a "Convo" window which holds messages from both users, and an "Input"
 *  window which takes input from the local user. All implementations must
 *  provide this minimum API.
 */

class ChatWindow {
    public:
       virtual void          clearConvo() = 0;
       virtual void          writeToConvo(std::string) = 0;
       virtual bool          hasPendingInput() = 0;
       virtual std::string   getUserInput() = 0;
};

#endif
