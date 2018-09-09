#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

class Logger {
    public:
        static Logger* getLogger();
        void log(std::string);
        
    private:
        Logger();
        ~Logger();
        static Logger* singelton;
        std::ofstream  logFile;
};

#endif
