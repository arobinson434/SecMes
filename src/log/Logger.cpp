#include "log/Logger.h"

Logger::Logger() {
#ifndef TEST
    logFile.open("log.txt", std::ios::app);
#else
    logFile.open("test-log.txt", std::ios::app);
#endif
    //freopen("log.txt", "a", stderr); //TODO: see how terrible this actually is
}

Logger::~Logger() {
    logFile.close();
}

Logger* Logger::getLogger() {
    if ( singelton == 0 ) {
        singelton = new Logger();
    }
    return singelton;
}

void Logger::log(std::string msg) {
    logFile << msg << std::endl;
}

Logger* Logger::singelton = 0;
