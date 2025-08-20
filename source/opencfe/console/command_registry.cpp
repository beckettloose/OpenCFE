#include "console.h"
#include "command_registry.h"
#include "completable_command.h"
#include <string>
#include <vector>

void CommandRegistry::registerCommand(const std::string &name, const std::string &help, std::function<void(const std::string&)> handler, CompletableCommand* completer) {
    commands.push_back({name, help, handler, completer});
}

const Command* CommandRegistry::findCommand(const std::string &prefix) const {
    std::vector<const Command*> matches;
    for (auto& cmd : commands ) {
        if (cmd.name.rfind(prefix, 0) == 0) {
            matches.push_back(&cmd);
        }
    }
    if (matches.size() == 1) return matches[0];
    return nullptr;
}

std::vector<std::string> CommandRegistry::complete(const std::string &prefix) const {
    std::vector<std::string> matches;
    for(auto &cmd : commands) {
        if (cmd.name.rfind(prefix, 0) == 0) {
            matches.push_back(cmd.name);
        }
    }
    return matches;
}

void CommandRegistry::printHelp() const {
    Console *con = Console::getInstance();
    con->write("Available commands:\r\n");
    for(auto &c : commands) {
        std::string line = "  " + c.name + " - " + c.help + "\r\n";
        con->write(line.c_str(), line.size());
    }
}
