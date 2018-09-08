#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <string>

class ChatWindow {
    public:
       //ChatWindow();
       //~ChatWindow();

       virtual void          writeToConvo(std::string) = 0;
       virtual std::string   readFromInput() = 0;
};

#endif
