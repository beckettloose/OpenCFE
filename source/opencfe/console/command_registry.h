#ifndef __COMMAND_REISTRY_H
#define __COMMAND_REISTRY_H

#include "completable_command.h"
#include <functional>

/**
* A console command with a name, help text, handler function, and optional
* autocomplete handler
*/
struct Command {

    // The command's name. This is what the user will type to execute it.
    std::string name;
    //
    // Basic description of the command. Should be about a sentence.
    std::string help;

    // Function to execute when the command is called.
    std::function<void(const std::string& args)> handler;

    // Optional completion handler for subcommands.
    CompletableCommand* completer = nullptr;
};

/**
* Keeps track of commands, and provides prefix find/autocomplete capability.
*/
class CommandRegistry {
public:
    /**
    * Add a command to the registry.
    */
    void registerCommand(const std::string &name, const std::string &help, std::function<void(const std::string& args)> handler, CompletableCommand* completer = nullptr);

    /**
    * Attempt to find a command by matching the beginning.
    * Returns nullptr if ambiguous or not found.
    */
    const Command* findCommand(const std::string &prefix) const;

    // Return a list of completions for a prefix
    std::vector<std::string> complete(const std::string &prefix) const;

    /**
    * Print a list of currently available commands.
    */
    void printHelp() const;

    // Return all commands
    const std::vector<Command>& list() const { return commands; }

private:
    std::vector<Command> commands;
};

#endif // !__COMMAND_REISTRY_H
