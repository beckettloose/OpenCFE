#include "lcd.h"
#include "EventQueue.h"
#include "canbus.h"
#include "../subsystem.h"
#include "mbed_shared_queues.h"

namespace Volvo {

LCD::LCD() {
    EventQueue *queue = mbed_event_queue();
    periodicEvent = new Event<void()>(queue, callback(this, &Volvo::LCD::periodic));
    periodicEvent->period(50ms);
}

void LCD::enable() {
    if (!enabled) return;

    // Turn on display, but media controls are disabled
    uint8_t enableStep1[8] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05};
    addToQueue(LCD_CONTROL_MESSAGE, enableStep1);

    // keep display enabled, but reactivate media controls
    uint8_t enableStep2[8] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    addToQueue(LCD_CONTROL_MESSAGE, enableStep2);
}

void LCD::disable() {
    if (!enabled) return;

    uint8_t disable[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    addToQueue(LCD_CONTROL_MESSAGE, disable);
}

void LCD::clear() {
    if (!enabled) return;

    uint8_t clear[8] = {0xE1, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    addToQueue(LCD_DATA_MESSAGE, clear);
}

void LCD::clear_force() {
    uint8_t clear[8] = {0xE1, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    CANPacket packet;
    packet.id = LCD_DATA_MESSAGE;
    memcpy(&packet.data, &clear[0], 8 * sizeof(uint8_t));
    canbus->tx_q_ls(&packet);
}

void LCD::update(const char text[], uint8_t len, uint8_t pos) {
    if (!enabled) return;

    char str[8] = {0};
    memcpy(str, text, len);
    char firstByte = 0xE0 + len;
    uint8_t data[8] = {
        firstByte,
        pos,
        str[0],
        str[1],
        str[2],
        str[3],
        str[4],
        str[5]
    };
    addToQueue(LCD_DATA_MESSAGE, data);
}

void LCD::print(const char text[], int len) {
    if (!enabled) return;

    char str[34] = {0};
    std::fill_n(str, 34, 0x20);
    memcpy(str, text, len);
    int block = 0;
    for (int i = 0; i < 32; i+=6) {
        switch (block) {
            case 0: {
                uint8_t d0[8] = {
                    0xA7,
                    0x00,
                    str[i],
                    str[i+1],
                    str[i+2],
                    str[i+3],
                    str[i+4],
                    str[i+5],
                };
                addToQueue(LCD_DATA_MESSAGE, d0);
                break;
            }
            case 4: {
                uint8_t d4[8] = {
                    0xA7,
                    str[i],
                    str[i+1],
                    str[i+2],
                    str[i+3],
                    str[i+4],
                    str[i+5],
                    str[i+6],
                };
                addToQueue(LCD_DATA_MESSAGE, d4);
                break;
            }
            default: {
                if (block != 5) {
                    char firstByte = 0x20 + block;
                    uint8_t d4[8] = {
                        firstByte,
                        str[i],
                        str[i+1],
                        str[i+2],
                        str[i+3],
                        str[i+4],
                        str[i+5],
                        str[i+6],
                    };
                    addToQueue(LCD_DATA_MESSAGE, d4);
                    i++;
                }
                break;
            }
        }
        block++;
    }
}

void LCD::addToQueue(uint32_t id, uint8_t data[8]) {
    if (lcd_data_queue.full()) return;

    CANPacket *packet = lcd_data_queue.try_alloc();

    memcpy(packet->data, data, sizeof(packet->data));
    packet->id = id;

    lcd_data_queue.put(packet);
}

void LCD::purgeQueue() {
    while (!lcd_data_queue.empty()) {
        CANPacket *packet = lcd_data_queue.try_get();
        if (packet == nullptr) break;
        lcd_data_queue.free(packet);
    }
}

void LCD::disable_force() {
    uint8_t disable[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    CANPacket packet;
    packet.id = LCD_CONTROL_MESSAGE;
    memcpy(&packet.data, &disable[0], 8 * sizeof(uint8_t));
    canbus->tx_q_ls(&packet);
}

void LCD::start() {
    purgeQueue();
    clear_force();
    disable_force();
    periodicEvent->post();
    enabled = true;
}

void LCD::stop() {
    enabled = false;
    periodicEvent->cancel();
    purgeQueue();
    clear_force();
    disable_force();
}

void LCD::periodic() {
    if (lcd_data_queue.empty()) return;

    CANPacket *packet = lcd_data_queue.try_get();
    if (packet == nullptr) return;

    canbus->tx_q_ls(packet);
    lcd_data_queue.free(packet);
}

}
