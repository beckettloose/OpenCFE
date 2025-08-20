#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <mbed.h>
#include "command_registry.h"
#include <string>
#include <vector>

#define CFE_CON_FLAG_RUN (1UL << 1) // Allows the console thread to run

/**
* A basic serial console implementation vibe coded with ChatGPT.
* I'll improve this later but I need some way to do high level debugging.
*/
class Console {
public:
    static Console* getInstance()  {
        static Console instance;
        return &instance;
    }

    /**
    * Attempt to parse and execute the user's command.
    */
    void processCommand();

    /**
    * Inform the command registry about a new command. This sets it up to handle
    * tab completion and basic help functionality, as well as allowing the
    * command to provide its own subcommand completion.
    */
    void registerCommand(const std::string &name, const std::string &help, std::function<void(const std::string& args)> handler, CompletableCommand* completer = nullptr);

    // Start the console thread
    void init();

    // Allow the console thread to begin running
    void start();

    // Tell tell the console thread to stop running
    void stop();

    /**
    * Write specified number of bytes from a buffer to the console.
    */
    void write(const void *buf, size_t len);

    /**
    * Write a byte array to the console.
    */
    void write(const char *msg);

protected:
    // The internal function executed by the console thread.
    void _threadTask();

    // The hardware serial instance used by the console.
    BufferedSerial* _rawSerial;
private:
    Console();

    // The thread that runs the console task.
    Thread* _thread;

    /**
    * Flag to signal enable/disable state to the console thread.
    * This has to be an EventFlags object so that we can enter low power sleep
    * while waiting for the flag to be set.
    */
    EventFlags* _flags;

    /**
    * Variable that tracks whether the console system is currently running.
    * Prevents repeated calls to start() and stop() from breaking things.
    */
    bool _hasStarted;

    static constexpr size_t MAX_BUFFER = 128; // TODO: delete?

    /**
    * Buffer to hold the command line text.
    */
    std::string inputBuffer;

    size_t index; // TODO: delete?

    /**
    * Registry to track base level commands.
    */
    CommandRegistry registry;

    /**
    * Print the prompt text to the screen "OpenCFE>".
    */
    void printPrompt();

    /**
    * Echo a single char to the screen.
    */
    void echoChar(char c);

    /**
    * Attempt to find a completion of the current command. Called when the user
    * presses the Tab key.
    */
    void handleTabCompletion();

    /**
    * Update the input buffer with the completed command.
    */
    void applyCompletion(const std::vector<std::string>& matches, const std::string& prefix);

    /**
    * Print a list of available commands.
    */
    void commandHelp(const std::string &args);
};

#endif // !__CONSOLE_H
