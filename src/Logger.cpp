#include "log/Logger.h"

Logger::Logger() {
    logFile.open("log.txt", std::ios::app);
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
