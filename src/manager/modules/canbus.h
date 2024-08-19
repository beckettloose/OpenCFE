#ifndef __CANBUS_H
#define __CANBUS_H

#include "manager/subsystem.h"
#include <cstdint>

#include <mbed.h>
#include <PinNames.h>
#include <vector>

#define CAN_RX_QUEUE_SIZE 50
#define CAN_TX_QUEUE_SIZE 25

struct CANPacket {
    uint32_t id;
    uint8_t data[8] = {0};
};

class CANbus : public Subsystem::Module {
public:
    class Listener {
        public:
            virtual void listener_update(CANPacket *packet);
    };

    static CANbus* getInstance() {
        static CANbus instance;
        return &instance;
    };
    void tx_ls(CANPacket *packet);
    void tx_hs(CANPacket *packet);
    void notify_listeners(CANPacket *packet);
    void addListener(CANbus::Listener *listener);

private:
    CANbus();
    CAN* CAN_ls;
    CAN* CAN_hs;

    void periodic();

    std::vector<CANbus::Listener*> rx_listeners;
    Mail<CANPacket, CAN_RX_QUEUE_SIZE> rx_queue;
    Mail<CANPacket, CAN_TX_QUEUE_SIZE> tx_queue;
    uint32_t last_can_rx = 0;

    CANMessage tx_frame;
    CANMessage rx_frame;

    Thread txrx_processor;
    void txrx_periodic();

    void tx_build_message(CANPacket *packet);
};

#endif
