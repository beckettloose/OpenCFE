#include "comms/canbus/canbus.h"

#include <cstring>
#include <mbed.h>
#include "libraries/LinkedList.h"

CAN CAN_ls(PD_0, PD_1);
CAN CAN_hs(PB_5, PB_6);

CANbus::CANbus() {
    rx_listeners = LinkedList<CANbus::Listener *>();

    // Set up the low speed bus (CAN LS, 125kbps)
    CAN_ls.frequency(125000);
    CAN_ls.attach(&on_can_rx_ls);

    // Set up the high speed bus (CAN HS, 250kbps [500 on MY05-])
    CAN_hs.frequency(250000);
    CAN_hs.attach(&on_can_rx_hs);
}

void CANbus::tx_build_message(CANPacket *packet) {
    tx_frame.format = CANFormat::CANExtended;
    tx_frame.id = packet->id;
    tx_frame.len = 8;
    memcpy(tx_frame.data, packet->data, sizeof(tx_frame.data));
}

void CANbus::tx_ls(CANPacket *packet) {
    tx_build_message(packet);
    CAN_ls.write(tx_frame);
}

void CANbus::tx_hs(CANPacket *packet) {
    tx_build_message(packet);
    CAN_hs.write(tx_frame);
}

void CANbus::on_can_rx_ls() {
    CAN_ls.read(getInstance()->rx_frame);
    getInstance()->process_rx();
}

void CANbus::on_can_rx_hs() {
    CAN_hs.read(getInstance()->rx_frame);
    getInstance()->process_rx();
}

void CANbus::process_rx() {
    // record timestamp of last message (TODO: which time source should we use?)
    last_can_rx = time(NULL);

    // exit if the message queue is full
    if (rx_queue.full()) return;

    // TODO: Validate the contents of the CAN frame?

    // allocate a slot in the rx message queue memory pool.
    CANPacket *packet = rx_queue.try_alloc();

    // copy the appropriate data to our allocated slot
    packet->id = rx_frame.id;
    memcpy(packet->data, rx_frame.data, sizeof(rx_frame.data));

    // put our allocated slot in to the queue
    rx_queue.put(packet);
}

void CANbus::periodic() {
    getInstance()->periodic_internal();
}

void CANbus::periodic_internal() {
    // get the next message in the can rx queue
    CANPacket *packet = rx_queue.try_get();

    // make sure we have a message
    if (packet != nullptr) {
        // do stuff with the message
        notify_listeners(packet);

        // free the memory
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
