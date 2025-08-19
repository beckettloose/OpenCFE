#include "lcd_welcome_msg.h"
#include "Event.h"
#include "EventQueue.h"
#include "ignition.h"
#include "lcd.h"
#include "mbed_shared_queues.h"
#include <cstring>

LCDWelcomeMessage::LCDWelcomeMessage() {
    subsystem = Subsystem::getInstance();
    lcd = Volvo::LCD::getInstance();
    ignition = Ignition::getInstance();
    lastKeyPos = Ignition::KeyOut;

    queue = mbed_event_queue();
    periodicEvent = new Event<void()>(queue, callback(this, &LCDWelcomeMessage::periodic));
    periodicEvent->period(250ms);

    showWelcomeEvent = new Event<void()>(queue, callback(this, &LCDWelcomeMessage::showWelcome));
    showWelcomeEvent->period(non_periodic);

    showGoodbyeEvent = new Event<void()>(queue, callback(this, &LCDWelcomeMessage::showGoodbye));
    showGoodbyeEvent->period(non_periodic);

    hideMessageEvent = new Event<void()>(queue, callback(this, &LCDWelcomeMessage::hideMessage));
    hideMessageEvent->delay(CFE_LCDWELCOME_TIME);
    hideMessageEvent->period(non_periodic);
}

void LCDWelcomeMessage::start() {
    periodicEvent->post();
}

void LCDWelcomeMessage::stop() {
    periodicEvent->cancel();
    showWelcomeEvent->cancel();
    showGoodbyeEvent->cancel();
    hideMessageEvent->cancel();
    hideMessage();
}

void LCDWelcomeMessage::periodic() {
    Ignition::KeyPos keyPos = ignition->getKeyPos();

    if (lastKeyPos == Ignition::KeyOut &&
        keyPos != Ignition::KeyOut) {
        showWelcomeEvent->post();
    }

    if (lastKeyPos == Ignition::KPI &&
        (keyPos == Ignition::KP0 || keyPos == Ignition::KeyOut)) {
        showGoodbyeEvent->post();
    }

    lastKeyPos = keyPos;
}

void LCDWelcomeMessage::showWelcome() {
    showGoodbyeEvent->cancel();
    hideMessageEvent->cancel();
    lcd->enable();
    lcd->clear();
    const char *text = "Welcome!";
    lcd->print(text, strlen(text));
    hideMessageEvent->post();
}

void LCDWelcomeMessage::showGoodbye() {
    showWelcomeEvent->cancel();
    hideMessageEvent->cancel();
    lcd->enable();
    lcd->clear();
    const char *text = "Goodbye!";
    lcd->print(text, strlen(text));
    hideMessageEvent->post();
}

void LCDWelcomeMessage::hideMessage() {
    lcd->clear();
    lcd->disable();
}
