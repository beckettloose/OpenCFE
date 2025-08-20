#include "console.h"
#include "../power_state_manager.h"
#include "lcd_command.h"
#include <sstream>

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

void Console::_threadTask() {
    write("\r\nWelcome to the OpenCFE Serial Console!\r\n");

    registerCommand("reset", "Reset the device",
                    [](const std::string &args) {
                        NVIC_SystemReset();
                    });
    registerCommand("echo", "Echo the provided text",
                         [this](const std::string &args) {
                         write(args.c_str(), args.size());
                         write("\r\n", 2);
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

    LCDCommand lcdCommand;

    registerCommand("lcd", "Control the DIM LCD", [&](const std::string &args){ lcdCommand(args); }, &lcdCommand);

    while (true) {
        // EventFlags.wait() allows us to enter low power sleep
        _flags->wait_all(CFE_CON_FLAG_RUN, osWaitForever, false);

        char c;
        while (_rawSerial->readable()) {
            // Ensure we read exactly 1 byte
            if (_rawSerial->read(&c, 1) != 1) continue;

            if (c == '\r' || c == '\n') {
                // Handle enter/return keys
                write("\r\n", 2);
                processCommand();
                inputBuffer.clear();
                printPrompt();
            } else if ((c == '\b' || c == 0x7F)) {
                // Handle backspace key
                if (!inputBuffer.empty()) {
                    inputBuffer.pop_back();
                    write("\b \b", 3);
                }
            } else if (c == '\t') {
                // Handle tab key
                handleTabCompletion();
            } else if (isprint(static_cast<unsigned char>(c))) {
                // Handle regular characters
                inputBuffer.push_back(c);
                write(&c, 1);
            }
        }
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

void Console::processCommand() {
    // Trim white space from start and end of input buffer
    auto first = inputBuffer.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return;
    auto last = inputBuffer.find_last_not_of(" \t\r\n");
    std::string line = inputBuffer.substr(first, last - first + 1);

    if (line.empty()) return;

    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    std::string args;
    std::getline(iss, args);
    if (!args.empty() && args[0] == ' ') args.erase(0, 1);

    if (cmd == "?") {
        registry.printHelp();
        return;
    }

    const Command* found = registry.findCommand(cmd);
    if (found) {
        if (args == "?") {
            if (found->completer) {
                found->completer->printHelp(cmd);
            } else {
                std::string msg = "No help available for " + found->name + "\r\n";
                write(msg.c_str(), msg.size());
            }
            return;
        }
        found->handler(args);
    } else {
        const char *msg = "Unknown or ambiguous command. Type 'help' or '?'\r\n";
        write(msg, std::strlen(msg));
    }
}

void Console::printPrompt() {
    const char *prompt = "OpenCFE>";
    write(prompt, std::strlen(prompt));
}

void Console::echoChar(char c) { write(&c, 1); }

void Console::registerCommand(const std::string &name, const std::string &help, std::function<void(const std::string& args)> handler, CompletableCommand* completer) {
    registry.registerCommand(name, help, handler, completer);
}

void Console::commandHelp(const std::string &) {
    const char *header = "Available commands:\r\n";
    write(header, std::strlen(header));
    for (auto &c : registry.list()) {
        std::string line = "  " + c.name + " - " + c.help + "\r\n";
        write(line.c_str(), line.size());
    }
}

void Console::handleTabCompletion() {
    std::istringstream iss(inputBuffer);
    std::string first;
    iss >> first;

    std::string rest;
    std::getline(iss, rest);
    if (!rest.empty() && rest[0] == ' ') rest.erase(0, 1);

    if (first.empty()) return;

    const Command* cmd = registry.findCommand(first);
    if (!cmd) {
        auto matches = registry.complete(first);
        applyCompletion(matches, first);
        return;
    }

    if (cmd->completer) {
        auto matches = cmd->completer->complete(rest);
        applyCompletion(matches, rest);
        return;
    }

    auto matches = registry.complete(first);
    applyCompletion(matches, first);
}

void Console::applyCompletion(const std::vector<std::string>& matches, const std::string& prefix) {
    if (matches.empty()) return;

    // BUG: Something in this logic needs to be fixed, applying completions is broken
    if (matches.size() == 1) {
        // replace suffix of buffer with match
        size_t pos = inputBuffer.rfind(prefix);
        if (pos != std::string::npos) {
            inputBuffer.replace(pos, prefix.size(), matches[0]);
            const char *seq = "\033[K"; // clear to end of line
            write(seq, std::strlen(seq));
            write(inputBuffer.c_str(), inputBuffer.size());
        }
    } else {
        const char *nl = "\r\n";
        write(nl, 2);
        for (auto &m : matches) {
            std::string line = "  " + m + "\r\n";
            write(line.c_str(), line.size());
        }
        printPrompt();
        write(inputBuffer.c_str(), inputBuffer.size());
    }
}

void Console::write(const void *buf, size_t len) {
    _rawSerial->write(buf, len);
}

void Console::write(const char *msg) {
    write(msg, std::strlen(msg));
}
