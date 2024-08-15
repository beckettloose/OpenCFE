#ifndef __CANBUS_H
#define __CANBUS_H

#include <cstdint>

#include <mbed.h>
#include <PinNames.h>
#include "libraries/LinkedList.h"

#define CAN_RX_QUEUE_SIZE 50

namespace Volvo {

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
        void tx_ls(CANPacket *packet);
        void tx_hs(CANPacket *packet);
        void process_rx(CANMessage message);
        static void periodic();
        void notify_listeners(CANPacket *packet);
        void addListener(CANbus::Listener *listener);
        EventQueue *rx_event_queue;

    private:
        CANbus();
        CAN* CAN_ls;
        CAN* CAN_hs;

        LinkedList<CANbus::Listener *> rx_listeners;
        Mail<CANPacket, CAN_RX_QUEUE_SIZE> rx_queue;
        uint32_t last_can_rx = 0;

        CANMessage tx_frame;
        CANMessage rx_frame;

        Thread rx_event_processor;

        void tx_build_message(CANPacket *packet);
        void periodic_internal();
        static void on_can_rx_ls();
        static void on_can_rx_hs();
};

}

#endif
