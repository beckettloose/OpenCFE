#ifndef __LCD_H
#define __LCD_H

#include "canbus/canbus.h"
#include "modules/ModuleBase.h"
#include <cstdint>

#define LCD_CONTROL_MESSAGE 0x0160200E
#define LCD_DATA_MESSAGE 0x00800008
#define LCD_MESSAGE_DELAY 40
#define LCD_DATA_QUEUE_LENGTH 20

namespace Volvo {

class LCD : public ModuleBase {
    public:
        static LCD* getInstance() {
            static LCD instance;
            return &instance;
        }
        void enable();
        void disable();
        void clear();
        void update(char text[], uint8_t len, uint8_t pos);
        void print(char text[], int len);
        void periodic();
    private:
        LCD();
        CANbus *canbus = CANbus::getInstance();
        Mail<CANPacket, LCD_DATA_QUEUE_LENGTH> lcd_data_queue;
        uint64_t next_message_time = 0;
        CANPacket next_lcd_message;
        void addToQueue(uint32_t id, uint8_t data[8]);
};

}

#endif
