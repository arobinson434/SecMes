#include <fstream>
#include <limits>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "chat/ConfigChatState.h"
#include "chat/WaitingChatState.h"

#define DIR_FLAGS S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH

ConfigChatState::ConfigChatState(ChatMachine* machine):
    AbstractChatState(machine) {
    mModeName = "ConfigState";

    // Set the config directory & file
    passwd* pw = getpwuid(getuid());
    configDir  = std::string(pw->pw_dir) + "/.secmes";
    configFile = configDir + "/config";
}

AbstractChatState* ConfigChatState::run() {
    if ( !hasConfigDir() ) {
        if ( !createConfigDir() ) {
            setRunning(false);
            return NULL;
        }
    }

    if ( !hasConfig() )
       createConfig();

    loadConfig();

    return new WaitingChatState(mMachine);
}

bool ConfigChatState::hasConfigDir() {
    struct stat dirInfo;

    if ( stat(configDir.c_str(), &dirInfo) == 0 && \
         (dirInfo.st_mode & S_IFDIR) ) {
        log("Found existing config directory");
        return true;
    }

    log("Failed to find existing config directory: " + configDir);
    return false;
}

bool ConfigChatState::hasConfig() {
    struct stat fileInfo;

    if ( stat(configFile.c_str(), &fileInfo) == 0 && \
         (fileInfo.st_mode & S_IFREG) ) {
        log("Found existing config file");
        return true;
    }

    log("Failed to find existing config file: " + configFile);
    return false;
}

bool ConfigChatState::createConfigDir() {
    log("Creating the config directory");
    if ( mkdir(configDir.c_str(), DIR_FLAGS) != 0 ) {
        perror("ConfigDir");
        log("Failed to create config directory");
        return false;
    }

    log("Successfully created the config directory");
    return true;
}

void ConfigChatState::createConfig() {
    std::string   username, port;
    std::ofstream configStream;
    configStream.open(configFile);

    writeToConvo("System: Failed to find an existing config file at: "+configFile+"\n");
    writeToConvo("System: Starting configuration...\n");

    writeToConvo("\tPlease enter a username.\n");
    username = getInfo();
    writeToConvo("\t\tusername = "+username+"\n");

    writeToConvo("\tPlease enter the listening port\n");
    port = getInfo();
    writeToConvo("\t\tport = "+port+"\n");

    configStream << "#This file is auto generated; Manual editing is discouraged!" << std::endl;
    configStream << username << std::endl;
    configStream << port << std::endl;

    configStream.close();
    writeToConvo("System: Configuration successfully written\n");
}

void ConfigChatState::loadConfig() {
    writeToConvo("System: Loading config... ");

    std::string   username, port;
    std::ifstream configStream;
    configStream.open(configFile);

    configStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard the header
    configStream >> username;
    configStream >> port;

    log("From config, username = "+username);
    setName(username);
    log("From config, port = "+port);
    initialize(port);

    writeToConvo("Done\n");
}

std::string ConfigChatState::getInfo() {
    while ( !hasPendingInput() ) {
        eventLoopSleep();
    }
    return getUserInput();
}
