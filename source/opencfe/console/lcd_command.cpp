#include "lcd_command.h"
#include <sstream>
#include <cstring>

LCDCommand::LCDCommand() {
    con = Console::getInstance();
    lcd = Volvo::LCD::getInstance();

    _hasLock = false;

    registry.registerCommand("lock", "Attempt to acquire the LCD lock",
        [this](const std::string &args){ lcdLock(args); });

    registry.registerCommand("unlock", "Release the LCD lock",
        [this](const std::string &args){ lcdUnlock(args); });

    registry.registerCommand("enable", "Turn the DIM LCD On",
        [this](const std::string &args){ lcdEnable(args); });

    registry.registerCommand("disable", "Turn the DIM LCD Off",
        [this](const std::string &args){ lcdDisable(args); });

    registry.registerCommand("write", "Write a string to the DIM LCD",
        [this](const std::string &args){ lcdWrite(args); });

    registry.registerCommand("clear", "Clear the DIM LCD",
        [this](const std::string &args){ lcdClear(args); });
}

void LCDCommand::operator()(const std::string &args) {
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
            std::string line = "Usage: lcd " + found->name + " <args>\r\n";
            con->write(line.c_str(), line.size());
            return;
        }

        found->handler(subArgs);
    } else {
        con->write("Unknown or ambiguous subcommand. Type 'lcd ?'\r\n");
    }
}

std::vector<std::string> LCDCommand::complete(const std::string &prefix) const {
    return registry.complete(prefix);
}

void LCDCommand::printHelp(const std::string&) const {
    registry.printHelp();
}

void LCDCommand::lcdLock(const std::string &args) {
    if (args.length() != 0) {
        con->write("error: lcd lock does not take any arguments\r\n");
        return;
    }

    if (_hasLock) {
        con->write("error: console already has lcd lock\r\n");
        return;
    }

    con->write("Waiting on LCD lock for up to 5 seconds... ");
    if (!lcd->mutex->trylock_for(5s)) {
        con->write("Failed!\r\n");
        return;
    }

    _hasLock = true;
    con->write("Success!\r\n");
}

void LCDCommand::lcdUnlock(const std::string &args) {
    if (args.length() != 0) {
        con->write("error: lcd unlock does not take any requirements\r\n");
        return;
    }

    if (!_hasLock) {
        con->write("error: console does not have lcd lock\r\n");
        return;
    }

    lcd->mutex->unlock();
    _hasLock = false;
}

void LCDCommand::lcdEnable(const std::string &args) {
    if (args.length() != 0) {
        con->write("error: lcd enable does not take any arguments\r\n");
        return;
    }

    if (!_hasLock) {
        con->write("error: need lcd lock to perform this function\r\n");
        return;
    }

    lcd->enable();
}

void LCDCommand::lcdDisable(const std::string &args) {
    if (args.length() != 0) {
        con->write("error: lcd disable does not take any arguments\r\n");
        return;
    }

    if (!_hasLock) {
        con->write("error: need lcd lock to perform this function\r\n");
        return;
    }

    lcd->disable();
}

void LCDCommand::lcdWrite(const std::string &args) {
    if (args.length() == 0 || args.length() > 32) {
        con->write("error: lcd write must receive between 1 and 32 characters!\r\n");
        return;
    }

    if (!_hasLock) {
        con->write("error: need lcd lock to perform this function\r\n");
        return;
    }

    lcd->print(args.c_str(), args.length());
}

void LCDCommand::lcdClear(const std::string &args) {
    if (args.length() != 0) {
        con->write("error: lcd clear does not take any arguments\r\n");
        return;
    }

    if (!_hasLock) {
        con->write("error: need lcd lock to perform this function\r\n");
        return;
    }

    lcd->clear();
}
