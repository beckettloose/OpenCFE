#ifndef __LCD_COMMAND_H
#define __LCD_COMMAND_H

#include "command_registry.h"
#include "completable_command.h"
#include "console.h"
#include <string>
#include <vector>
#include "../modules/lcd.h"

/**
* Command group for controlling the DIM LCD.
*/
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

    /**
    * Attempt to acquire the LCD mutex lock for up to 5 seconds.
    */
    void lcdLock(const std::string &args);

    /**
    * Release the LCD mutex lock.
    */
    void lcdUnlock(const std::string &args);

    /**
    * Enable the LCD.
    */
    void lcdEnable(const std::string &args);

    /**
    * Disable the LCD.
    */
    void lcdDisable(const std::string &args);

    /**
    * Write arbitrary text to the LCD.
    */
    void lcdWrite(const std::string &args);

    /**
    * Clear all text from the LCD.
    */
    void lcdClear(const std::string &args);
};

#endif // !__LCD_COMMAND_H
