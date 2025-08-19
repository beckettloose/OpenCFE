#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "EventFlags.h"
#include "command_registry.h"
#include <mbed.h>
#include <string>

#define CFE_CON_FLAG_RUN (1UL << 1) // Allows the console thread to run

/**
* A basic serial console written with the help of ChatGPT for now.
* I'll improve this later but I need some way to do high level debugging.
*/
class Console {
public:
    static Console* getInstance()  {
        static Console instance;
        return &instance;
    }

    bool processInput(std::string &outCommand);

    void registerCommand(const std::string &name, const std::string &help, CommandRegistry::CommandHandler handler);

    void init();
    void start();
    void stop();

    void write(const void *buf, size_t len);
    void write(const char *msg);

protected:
    void _threadTask();
    BufferedSerial* _rawSerial;
private:
    Console();

    Thread* _thread;

    EventFlags* _flags;

    bool _hasStarted;

    static constexpr size_t MAX_BUFFER = 128;
    char buffer[MAX_BUFFER];
    size_t index;

    CommandRegistry registry;

    void printPrompt();
    void echoChar(char c);
    void handleCommand(const std::string &line);
    void handleTabCompletion();

    void commandHelp(const std::string &args);
};

#endif // !__CONSOLE_H
