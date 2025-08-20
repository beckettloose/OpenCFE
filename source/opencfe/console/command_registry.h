#ifndef __COMMAND_REISTRY_H
#define __COMMAND_REISTRY_H

#include "completable_command.h"
#include <string>
#include <vector>
#include <functional>
#include <optional>

struct Command {
    std::string name;
    std::string help;
    std::function<void(const std::string& args)> handler;
    CompletableCommand* completer = nullptr; // optional
};

class CommandRegistry {
public:
    void registerCommand(const std::string &name, const std::string &help, std::function<void(const std::string& args)> handler, CompletableCommand* completer = nullptr);

    // Attempt to find a command by matching the beginning.
    // Returns nullptr if ambiguous or not found
    const Command* findCommand(const std::string &prefix) const;

    // Return a list of completions for a prefix
    std::vector<std::string> complete(const std::string &prefix) const;

    void printHelp() const;

    // Return all commands
    const std::vector<Command>& list() const { return commands; }

private:
    std::vector<Command> commands;
};

#endif // !__COMMAND_REISTRY_H
