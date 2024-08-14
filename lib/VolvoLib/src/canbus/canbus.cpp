#include "canbus.h"

#include <cstring>
#include <mbed.h>
#include "libraries/LinkedList.h"

namespace Volvo {

CANbus::CANbus() {
    rx_listeners = LinkedList<CANbus::Listener *>();

    // Set up the low speed bus (CAN LS, 125kbps)
    CAN_ls = new CAN(PD_0, PD_1);
    CAN_ls->frequency(125000);
    CAN_ls->attach(&on_can_rx_ls, CAN::RxIrq);

    // Set up the high speed bus (CAN HS, 250kbps [500 on MY05-])
    CAN_hs = new CAN(PB_5, PB_6);
    CAN_hs->frequency(250000);
    CAN_hs->attach(&on_can_rx_hs, CAN::RxIrq);

}

void CANbus::tx_build_message(CANPacket *packet) {
    tx_frame.format = CANFormat::CANExtended;
    tx_frame.id = packet->id;
    tx_frame.len = 8;
    memcpy(tx_frame.data, packet->data, sizeof(tx_frame.data));
}

void CANbus::tx_ls(CANPacket *packet) {
    tx_build_message(packet);
    CAN_ls->write(tx_frame);
}

void CANbus::tx_hs(CANPacket *packet) {
    tx_build_message(packet);
    CAN_hs->write(tx_frame);
}

void CANbus::process_rx(CANMessage message) {
    // record timestamp of last message (TODO: which time source should we use?)
    last_can_rx = time(NULL);
    if (rx_queue.full()) return;
    CANPacket *packet = rx_queue.try_alloc();
    packet->id = rx_frame.id;
    memcpy(packet->data, rx_frame.data, sizeof(rx_frame.data));
    rx_queue.put(packet);
}

void CANbus::periodic() {
    getInstance()->periodic_internal();
}

void CANbus::periodic_internal() {
    CANPacket *packet = rx_queue.try_get();

    if (packet != nullptr) {
        notify_listeners(packet);
        rx_queue.free(packet);
   }
}

void CANbus::notify_listeners(CANPacket *packet) {
    for (int i = 0; i < rx_listeners.size(); i++) {
        CANbus::Listener *listener = rx_listeners.get(i);
        if ((packet->id & listener->listener_params.mask) != 0) {
            listener->listener_update(packet);
        }
    }
}

void CANbus::addListener(CANbus::Listener *listener) {
    rx_listeners.add(listener);
}

void CANbus::on_can_rx_hs() {
    // TODO: copy can messages to queue
}

void CANbus::on_can_rx_ls() {
    // TODO: copy can messages to queue
}

}
