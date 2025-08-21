#include "logging.h"

Logging::Logging() {
    con = Console::getInstance();
}

void Logging::debug(std::string module, std::string message) {
    if (_consoleLogLevel <= LOG_LEVEL_DEBUG) {
        con->loggerWrite("[DEBUG] " + module + ": " + message + "\r\n");
    }
}

void Logging::info(std::string module, std::string message) {
    if (_consoleLogLevel <= LOG_LEVEL_INFO) {
        con->loggerWrite("[INFO] " + module + ": " + message + "\r\n");
    }
}

void Logging::warn(std::string module, std::string message) {
    if (_consoleLogLevel <= LOG_LEVEL_WARN) {
        con->loggerWrite("[WARN] " + module + ": " + message + "\r\n");
    }
}

void Logging::error(std::string module, std::string message) {
    if (_consoleLogLevel <= LOG_LEVEL_ERROR) {
        con->loggerWrite("[ERROR] " + module + ": " + message + "\r\n");
    }
}
