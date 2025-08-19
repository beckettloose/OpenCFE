#include "console.h"
#include "../power_state_manager.h"
#include "BufferedSerial.h"
#include "Callback.h"
#include "EventFlags.h"
#include "PinNameAliases.h"
#include "Thread.h"
#include <cstring>
#include <sstream>
#include <string>

Console::Console() : index(0) {
    _rawSerial = new BufferedSerial(USBTX, USBRX);
    _rawSerial->enable_input(false);
    _rawSerial->set_baud(115200);
    _rawSerial->set_format(8, BufferedSerial::None, 1); // 8N1

    _flags = new EventFlags;
    _flags->clear();

    registerCommand("help", "List available commands",
                    [this](const std::string &args) { commandHelp(args); });

    _thread = new Thread;
}

void resetCommand(const std::string &args) {
    NVIC_SystemReset();
}

void Console::_threadTask() {
    write("\r\nWelcome to the OpenCFE Serial Console!\r\n");

    registerCommand("reset", "Reset the device", resetCommand);
    registerCommand("echo", "Echo the provided text",
                         [this](const std::string &args) {
                         _rawSerial->write(args.c_str(), args.size());
                         _rawSerial->write("\r\n", 2);
                         });

    registerCommand("caffeinate", "Prevent the system from sleeping",
                         [this](const std::string &args) {
                            write("Caffeinating...\r\n");
                            PowerStateManager::getInstance()->caffeinate();
                         });

    registerCommand("decaffeinate", "Allow the system to sleep",
                         [this](const std::string &args){
                            write("Decaffeinating...\r\n");
                            PowerStateManager::getInstance()->decaffeinate();
                         });

    std::string cmd;
    while (true) {
        _flags->wait_all(CFE_CON_FLAG_RUN, osWaitForever, false);
        processInput(cmd);
    }
}

void Console::init() {
    _thread->start(callback(this, &Console::_threadTask));
}

void Console::start() {
    if (!_hasStarted) {
        _flags->set(CFE_CON_FLAG_RUN);
        _rawSerial->enable_input(true);
        write("\r\n\nOpenCFE>");
        _hasStarted = true;
    }
}

void Console::stop() {
    if (_hasStarted) {
        _flags->clear(CFE_CON_FLAG_RUN);
        _rawSerial->enable_input(false);
        write("\r\nGoing to Sleep...\r\n");
        _hasStarted = false;
    }
}

bool Console::processInput(std::string &outCommand) {
    if (!_rawSerial->readable()) {
        return false;
    }

    char c;
    if (_rawSerial->read(&c, 1) == 0) {
        return false;
    }

    if (c == '\r' || c == '\n') {
        _rawSerial->write("\r\n", 2);
        buffer[index] = '\0';
        outCommand = std::string(buffer);
        handleCommand(outCommand);
        index = 0;
        printPrompt();
        return true;
    }

    if ((c == '\b' || c == 127) && index > 0) {
        index--;
        _rawSerial->write("\b \b", 3);
        return false;
    }

    if (c == '\t') {
        handleTabCompletion();
        return false;
    }

    if (index < MAX_BUFFER - 1 && c >= 32 && c < 127) {
        buffer[index++] = c;
        echoChar(c);
    }

    return false;
}

void Console::printPrompt() {
    const char *prompt = "OpenCFE>";
    _rawSerial->write(prompt, std::strlen(prompt));
}

void Console::echoChar(char c) { _rawSerial->write(&c, 1); }

void Console::registerCommand(const std::string &name, const std::string &help, CommandRegistry::CommandHandler handler) {
    registry.registerCommand(name, help, handler);
}

void Console::handleCommand(const std::string &line) {
    if (line.empty())
        return;

    std::istringstream iss(line);
    std::string cmd;
    std::getline(iss, cmd, ' ');

    std::string args;
    std::getline(iss, args);
    if (!args.empty() && args[0] == ' ') args.erase(0, 1);

    auto found = registry.findCommand(cmd);
    if (found) {
        found->handler(args);
    } else {
        const char *msg = "Unknown command. Type 'help' for list.\r\n";
        write(msg);
    }
}

void Console::commandHelp(const std::string &) {
    const char *header = "Available commands:\r\n";
    _rawSerial->write(header, std::strlen(header));
    for (auto &c : registry.list()) {
        std::string line = "  " + c.name + " - " + c.help + "\r\n";
        write(line.c_str(), line.size());
    }
}

void Console::handleTabCompletion() {
    buffer[index] = '\0';
    std::string current(buffer);

    auto matches = registry.complete(current);
    if (matches.empty()) return;

    if (matches.size() == 1) {
        const std::string &match = matches[0];
        if (match.size() > current.size()) {
            std::string suffix = match.substr(current.size());
            for (char ch : suffix) {
                if (index < MAX_BUFFER - 1) {
                    buffer[index++] = ch;
                    echoChar(ch);
                }
            }
        }
    } else {
        write("\r\n");
        for (auto &m : matches) {
            std::string line = "  " + m + "\r\n";
            write(line.c_str(), line.size());
        }
        write("OpenCFE>");
        write(buffer, index);
    }
}

void Console::write(const void *buf, size_t len) {
    _rawSerial->write(buf, len);
}

void Console::write(const char *msg) {
    write(msg, std::strlen(msg));
}
