#ifndef __LCD_WELCOME_MSG_H
#define __LCD_WELCOME_MSG_H

#include <mbed.h>

#include "manager/modules/ignition.h"
#include "manager/modules/lcd.h"
#include "manager/subsystem.h"

#define CFE_LCDWELCOME_TIME 5s
#define CFE_LCDWELCOME_KPII_HIDE true

class LCDWelcomeMessage : public Subsystem::Module {
public:
    static LCDWelcomeMessage* getInstance() {
        static LCDWelcomeMessage instance;
        return &instance;
    }

    void start();
    void stop();
private:
    LCDWelcomeMessage();
    Subsystem* subsystem;
    Volvo::LCD* lcd;
    Ignition* ignition;
    Ignition::KeyPos lastKeyPos;

    Event<void()>* periodicEvent;
    void periodic();

    Event<void()>* showWelcomeEvent;
    void showWelcome();

    Event<void()>* showGoodbyeEvent;
    void showGoodbye();

    Event<void()>* hideMessageEvent;
    void hideMessage();
};

#endif
