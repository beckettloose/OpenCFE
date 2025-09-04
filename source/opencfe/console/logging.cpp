#include "logging.h"
#include <sstream>

Logging::Logging() {
    con = Console::getInstance();
}

void Logging::debug(std::string module, std::string message) {
    if (_consoleLogLevel <= LOG_LEVEL_DEBUG
        && (module == _consoleModuleFilter || _consoleModuleFilter.empty())) {
        con->loggerWrite("[DEBUG] " + module + ": " + message + "\r\n");
    }
}

void Logging::info(std::string module, std::string message) {
    if (_consoleLogLevel <= LOG_LEVEL_INFO
        && (module == _consoleModuleFilter || _consoleModuleFilter.empty())) {
        con->loggerWrite("[INFO] " + module + ": " + message + "\r\n");
    }
}

void Logging::warn(std::string module, std::string message) {
    if (_consoleLogLevel <= LOG_LEVEL_WARN
        && (module == _consoleModuleFilter || _consoleModuleFilter.empty())) {
        con->loggerWrite("[WARN] " + module + ": " + message + "\r\n");
    }
}

void Logging::error(std::string module, std::string message) {
    if (_consoleLogLevel <= LOG_LEVEL_ERROR
        && (module == _consoleModuleFilter || _consoleModuleFilter.empty())) {
        con->loggerWrite("[ERROR] " + module + ": " + message + "\r\n");
    }
}

void Logging::setLogLevel(uint8_t logLevel) {
    _consoleLogLevel = logLevel;
}

void Logging::setLogModule(std::string module) {
    _consoleModuleFilter = module;
}

Logging::LogCommand::LogCommand() {
    SetLogLevelCommand levelCommand;
    registry.registerCommand("level", "Set the log level required for a message to be printed",
                             [&](const std::string &args){ levelCommand(args); }, &levelCommand);

    registry.registerCommand("module", "Choose the module to filter log messages by",
                             [](const std::string &args){
                                Logging::getInstance()->setLogModule(args);
                             });
}

void Logging::LogCommand::operator()(const std::string &args) {
    std::istringstream iss(args);
    std::string subcmd;
    iss >> subcmd;

    if (subcmd.empty() || subcmd == "?") {
        registry.printHelp();
        return;
    }

    const auto* found = registry.findCommand(subcmd);
    if (found) {
        std::string subArgs;
        std::getline(iss, subArgs);
        if (!subArgs.empty() && subArgs[0] == ' ') subArgs.erase(0, 1);

        if (subArgs == "?") {
            std::string line = "Usage: log " + found->name + " <args> \r\n";
            con->write(line.c_str(), line.size());
            return;
        }

        found->handler(subArgs);
    } else {
        con->write("Unknown or ambiguous command. Type 'log ?'\r\n");
    }
}

std::vector<std::string> Logging::LogCommand::complete(const std::string &prefix) const {
    return registry.complete(prefix);
}

void Logging::LogCommand::printHelp(const std::string&) const {
    registry.printHelp();
}
Logging::SetLogLevelCommand::SetLogLevelCommand() {
    registry.registerCommand("debug", "Show messages with a log level of debug or greater",
                             [](const std::string &args){
                                 Logging::getInstance()->setLogLevel(LOG_LEVEL_DEBUG);
                                 Logging::getInstance()->info("logger", "log level changed to debug (0)");
                             });

    registry.registerCommand("info", "Show messages with a log level of info or greater",
                             [](const std::string &args) {
                                 Logging::getInstance()->setLogLevel(LOG_LEVEL_INFO);
                                 Logging::getInstance()->info("logger", "log level changed to info (1)");
                             });

    registry.registerCommand("warn", "Show messages with a log level of warn or greater",
                             [](const std::string &args) {
                                 Logging::getInstance()->setLogLevel(LOG_LEVEL_WARN);
                                 Logging::getInstance()->info("logger", "log level changed to warn (2)");
                             });

    registry.registerCommand("error", "Show messages with a log level of error or greater",
                             [](const std::string &args) {
                                 Logging::getInstance()->setLogLevel(LOG_LEVEL_ERROR);
                                 Logging::getInstance()->info("logger", "log level changed to error (3)");
                             });

    registry.registerCommand("none", "Don't show messages of any log level",
                             [](const std::string &args) {
                                 Logging::getInstance()->setLogLevel(LOG_LEVEL_NONE);
                                 Logging::getInstance()->info("logger", "set log level to none (255)");
                             });
}

void Logging::SetLogLevelCommand::operator()(const std::string &args) {
    std::istringstream iss(args);
    std::string subcmd;
    iss >> subcmd;

    if (subcmd.empty() || subcmd == "?") {
        registry.printHelp();
        return;
    }

    const auto* found = registry.findCommand(subcmd);
    if (found) {
        std::string subArgs;
        std::getline(iss, subArgs);
        if (!subArgs.empty() && subArgs[0] == ' ') subArgs.erase(0, 1);

        if (subArgs == "?") {
            std::string line = "Usage: log level " + found->name + "\r\n";
            con->write(line.c_str(), line.size());
            return;
        }

        found->handler(subArgs);
    } else {
        con->write("Unknown or ambiguous command. Type 'log level ?'\r\n");
    }
}

std::vector<std::string> Logging::SetLogLevelCommand::complete(const std::string &prefix) const {
    return registry.complete(prefix);
}

void Logging::SetLogLevelCommand::printHelp(const std::string&) const {
    registry.printHelp();
}
