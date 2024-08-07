#ifndef __CANBUS_H
#define __CANBUS_H

#include <cstdint>

#include <mbed.h>
#include "libraries/LinkedList.h"

#define CAN_RX_QUEUE_SIZE 50

struct CANPacket {
    uint32_t id;
    uint8_t data[8] = {0};
};

class CANbus {
    public:
        class Listener {
            struct ListenerParams {
                uint32_t mask = (1 << 29) - 1; // default to all messages
            };
            public:
                virtual void listener_update(CANPacket *packet);
                ListenerParams listener_params;
        };

        static CANbus* getInstance() {
            static CANbus instance;
            return &instance;
        };
        CANbus();
        void tx_ls(CANPacket *packet);
        void tx_hs(CANPacket *packet);
        void process_rx();
        static void periodic();
        void notify_listeners(CANPacket *packet);
        void addListener(CANbus::Listener *listener);

    private:
        LinkedList<CANbus::Listener *> rx_listeners;
        Mail<CANPacket, CAN_RX_QUEUE_SIZE> rx_queue;
        uint32_t last_can_rx = 0;

        CANMessage tx_frame;
        CANMessage rx_frame;

        void tx_build_message(CANPacket *packet);
        void periodic_internal();
        static void on_can_rx_ls();
        static void on_can_rx_hs();
};

#endif
