#include "console.h"
#include "../power_state_manager.h"
#include "BufferedSerial.h"
#include "Callback.h"
#include "PinNameAliases.h"
#include "Thread.h"
#include <cstring>
#include <sstream>

Console::Console() : index(0) {
    _rawSerial = new BufferedSerial(USBTX, USBRX);
    _rawSerial->set_baud(115200);
    _rawSerial->set_format(8, BufferedSerial::None, 1); // 8N1

    registerCommand("help", "List available commands",
                    [this](const std::string &args) { commandHelp(args); });

    _thread = new Thread;
}

void resetCommand(const std::string &args) {
    NVIC_SystemReset();
}

void Console::_threadTask() {
    _rawSerial->write("\r\n", 2);
    printPrompt();

    Console *con = Console::getInstance();

    con->registerCommand("reset", "Reset the device", resetCommand);
    con->registerCommand("echo", "Echo the provided text",
                         [this](const std::string &args) {
                         _rawSerial->write(args.c_str(), args.size());
                         _rawSerial->write("\r\n", 2);
                         });

    con->registerCommand("caffeinate", "Prevent the system from sleeping",
                         [this](const std::string &args) {
                            const char *msg = "Caffeinating...\r\n";
                            write(msg);
                            PowerStateManager::getInstance()->caffeinate();
                         });

    con->registerCommand("decaffeinate", "Allow the system to sleep",
                         [this](const std::string &args){
                            const char *msg = "Decaffeinating...\r\n";
                            write(msg);
                            PowerStateManager::getInstance()->decaffeinate();
                         });

    std::string cmd;
    while (true) {
        _flags->wait_all(CFE_CON_FLAG_RUN, osWaitForever, false);
        con->processInput(cmd);
    }
}

void Console::init() {
    _thread->start(callback(this, &Console::_threadTask));
}

void Console::start() {
    _flags->set(CFE_CON_FLAG_RUN);
}

void Console::stop() {
    _flags->clear(CFE_CON_FLAG_RUN);
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

void Console::registerCommand(const std::string &name, const std::string &help,
                              CommandHandler handler) {
    commands.push_back({name, help, handler});
}

void Console::handleCommand(const std::string &line) {
    if (line.empty())
        return;

    std::istringstream iss(line);
    std::string cmd;
    std::getline(iss, cmd, ' ');

    std::string args;
    std::getline(iss, args);
    if (!args.empty() && args[0] == ' ') {
        args.erase(0, 1);
    }

    for (auto &c : commands) {
        if (c.name == cmd) {
            c.handler(args);
            return;
        }
    }

    const char *msg = "Unknown command. Type 'help' for list.\r\n";
    write(msg);
}

void Console::commandHelp(const std::string &) {
    const char *header = "Available commands:\r\n";
    _rawSerial->write(header, std::strlen(header));
    for (auto &c : commands) {
        std::string line = "  " + c.name + " - " + c.help + "\r\n";
        write(line.c_str(), line.size());
    }
}

void Console::write(const void *buf, size_t len) {
    _rawSerial->write(buf, len);
}

void Console::write(const char *msg) {
    write(msg, std::strlen(msg));
}
