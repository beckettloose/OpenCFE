#ifndef __LCD_COMMAND_H
#define __LCD_COMMAND_H

#include "command_registry.h"
#include "completable_command.h"
#include "console.h"
#include <string>
#include <vector>
#include "../modules/lcd.h"

class LCDCommand : public CompletableCommand {
public:
    LCDCommand();

    void operator()(const std::string &args);

    std::vector<std::string> complete(const std::string &prefix) const override;
    void printHelp(const std::string& prefix) const override;

private:
    Console *con;
    Volvo::LCD *lcd;

    bool _hasLock;

    CommandRegistry registry;

    void lcdLock(const std::string &args);
    void lcdUnlock(const std::string &args);
    void lcdEnable(const std::string &args);
    void lcdDisable(const std::string &args);
    void lcdWrite(const std::string &args);
    void lcdClear(const std::string &args);
};

#endif // !__LCD_COMMAND_H
