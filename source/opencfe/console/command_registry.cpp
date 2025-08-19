#include "command_registry.h"

void CommandRegistry::registerCommand(const std::string &name, const std::string &help, CommandHandler handler) {
    commands.push_back({name, help, handler});
}

const CommandRegistry::Command* CommandRegistry::findCommand(const std::string &input) const {
    if (input.empty()) return nullptr;

    std::vector<const Command*> matches;

    for (auto &cmd : commands) {
        if (cmd.name == input) {
            return &cmd;
        }
        if (cmd.name.rfind(input, 0) == 0) {
            matches.push_back(&cmd);
        }
    }

    if (matches.size() == 1) {
        return matches[0];
    }

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
