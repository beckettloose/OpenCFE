#ifndef __LOGGING_H
#define __LOGGING_H

#include "console.h"

class Logging {
public:
    static Logging* getInstance() {
        static Logging instance;
        return &instance;
    }

    static constexpr uint8_t LOG_LEVEL_DEBUG = 0;
    static constexpr uint8_t LOG_LEVEL_INFO = 1;
    static constexpr uint8_t LOG_LEVEL_WARN = 2;
    static constexpr uint8_t LOG_LEVEL_ERROR = 3;
    static constexpr uint8_t LOG_LEVEL_NONE = 0xFF;

    void debug(std::string module, std::string message);
    void info(std::string module, std::string message);
    void warn(std::string module, std::string message);
    void error(std::string module, std::string message);

    void setLogLevel(uint8_t logLevel);
    void setLogModule(std::string module);

    class LogCommand : public CompletableCommand {
    public:
        LogCommand();

        void operator()(const std::string &args);

        std::vector<std::string> complete(const std::string &prefix) const override;
        void printHelp(const std::string &prefix) const override;

    private:
        Console *con;
        CommandRegistry registry;
    };

    class SetLogLevelCommand : public CompletableCommand {
    public:
        SetLogLevelCommand();

        void operator()(const std::string &args);

        std::vector<std::string> complete(const std::string &prefix) const override;
        void printHelp(const std::string &prefix) const override;

    private:
        Console *con;
        CommandRegistry registry;
    };

private:
    Logging();

    Console* con;

    // uint8_t _consoleLogLevel = LOG_LEVEL_NONE;
    uint8_t _consoleLogLevel = LOG_LEVEL_DEBUG;
    std::string _consoleModuleFilter = "";

    void commandSetLogModule(const std::string &args);
};

#endif // !__LOGGING_H

