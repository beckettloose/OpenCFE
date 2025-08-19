#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <mbed.h>
#include <string>
#include <functional>
#include <vector>

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

    using CommandHandler = std::function<void(const std::string &args)>;

    bool processInput(std::string &outCommand);

    void registerCommand(const std::string &name, const std::string &help, CommandHandler handler);

    void start();
protected:
    void _threadTask();
    BufferedSerial* _rawSerial;
private:
    Console();

    Thread* _thread;


    struct Command {
        std::string name;
        std::string help;
        CommandHandler handler;
    };

    static constexpr size_t MAX_BUFFER = 128;
    char buffer[MAX_BUFFER];
    size_t index;

    std::vector<Command> commands;

    void printPrompt();
    void echoChar(char c);
    void handleCommand(const std::string &line);

    void commandHelp(const std::string &args);
};

#endif // !__CONSOLE_H
