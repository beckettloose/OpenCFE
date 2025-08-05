#ifndef __LCD_H
#define __LCD_H

#include "canbus.h"
#include "../subsystem.h"
#include <cstdint>

#define LCD_CONTROL_MESSAGE 0x0160200E
#define LCD_DATA_MESSAGE 0x00800008
#define LCD_MESSAGE_DELAY 40
#define LCD_DATA_QUEUE_LENGTH 20

namespace Volvo {

class LCD : public Subsystem::Module {
public:
    static LCD* getInstance() {
        static LCD instance;
        return &instance;
    }
    void enable();
    void disable();
    void clear();
    void update(const char text[], uint8_t len, uint8_t pos);
    void print(const char text[], int len);
    void periodic();

    void start();
    void stop();
private:
    LCD();
    bool enabled = false;
    CANbus *canbus = CANbus::getInstance();
    Subsystem* subsystem = Subsystem::getInstance();
    Event<void()>* periodicEvent;
    Mail<CANPacket, LCD_DATA_QUEUE_LENGTH> lcd_data_queue;
    uint64_t next_message_time = 0;
    CANPacket next_lcd_message;
    void addToQueue(uint32_t id, uint8_t data[8]);
    void purgeQueue();
    void disable_force();
    void clear_force();
};

}

#endif
