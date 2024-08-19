#include "canbus.h"

#include <cstring>
#include <mbed.h>

CANbus::CANbus() {
    rx_listeners = std::vector<CANbus::Listener *>();

    // Set up the low speed bus (CAN LS, 125kbps)
    CAN_ls = new CAN(PD_0, PD_1);
    CAN_ls->frequency(125000);

    // Set up the high speed bus (CAN HS, 250kbps [500 on MY05-])
    CAN_hs = new CAN(PB_5, PB_6);
    CAN_hs->frequency(250000);
}

void CANbus::tx_ls(CANPacket *packet) {
    tx_build_message(packet);
    CAN_ls->write(tx_frame);
}

void CANbus::tx_hs(CANPacket *packet) {
    tx_build_message(packet);
    CAN_hs->write(tx_frame);
}

void CANbus::notify_listeners(CANPacket *packet) {
    for (auto & listener : rx_listeners) {
        listener->listener_update(packet);
    }
}

void CANbus::addListener(CANbus::Listener *listener) {
    rx_listeners.push_back(listener);
}

void CANbus::periodic() {
}

void CANbus::txrx_periodic() {
}

void CANbus::tx_build_message(CANPacket *packet) {
    tx_frame.format = CANFormat::CANExtended;
    tx_frame.id = packet->id;
    tx_frame.len = 8;
    memcpy(tx_frame.data, packet->data, sizeof(tx_frame.data));
}

