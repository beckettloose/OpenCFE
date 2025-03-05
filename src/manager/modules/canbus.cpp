#include "canbus.h"
#include "InterfaceCAN.h"
#include "can_helper.h"

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

    subsystem = Subsystem::getInstance();
    EventQueue *queue = mbed_event_queue();
    periodicEvent = new Event<void()>(queue, callback(this, &CANbus::periodic));
    periodicEvent->period(50ms);
}

void CANbus::tx_q_ls(CANPacket *packet) {
    if (allow_queue_tx) {
        // TODO: check if we can push to queue
        if (!tx_queue_ls.full()) {
            CANPacket *queuePacket = tx_queue_ls.try_alloc();
            queuePacket->id = packet->id;
            memcpy(&queuePacket->data[0], &packet->data[0], 8 * sizeof(uint8_t));
            tx_queue_ls.put(queuePacket);
        }
    }
}

void CANbus::tx_q_hs(CANPacket *packet) {
    if (allow_queue_tx){
        // TODO: check if we can push to queue
        if (!tx_queue_hs.full()) {
            CANPacket *queuePacket = tx_queue_hs.try_alloc();
            queuePacket->id = packet->id;
            memcpy(&queuePacket->data[0], &packet->data[0], 8 * sizeof(uint8_t));
            tx_queue_hs.put(queuePacket);
        }
    }
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

void CANbus::start() {
    canLED->write(1);
    periodicEvent->post();
    txrx_flags.set(CFE_CAN_FLAG_TXRX_RUN);
    allow_queue_tx = true;
}

void CANbus::stop() {
    allow_queue_tx = false;
    txrx_flags.set(CFE_CAN_FLAG_TXRX_SHUTDOWN);
    txrx_flags.wait_all(CFE_CAN_FLAG_MAIN_TXRX_CLEAN, 2000, false);
    periodicEvent->cancel();
    canLED->write(0);
}

void CANbus::addRxFilter(Bus bus, uint32_t id, uint32_t mask) {
    switch (bus) {
        case LS: {
            CAN_ls->filter(id, mask, CANFormat::CANExtended);
            break;
        };
        case HS: {
            CAN_hs->filter(id, mask, CANFormat::CANExtended);
            break;
        };
        default: {
            // TODO: error
            break;
        };
    }
}

void CANbus::periodic() {
    // update our listeners with every queued message
    // reads up to a predetermined number of messages from the queue at a time.
    // exit immediately if queue is empty at any point.

    for (int i = 0; i < CAN_RX_QUEUE_MESSAGES_PER_PERIOD; i++) {
        if (rx_queue.empty()) break;

        CANPacket* packet = rx_queue.try_get();
        for (CANbus::Listener* l : rx_listeners) {
            l->listener_update(packet);
        }
        rx_queue.free(packet);
    }
}

void CANbus::txrx_loop() {
    while (true) {
        txrx_flags.wait_all(CFE_CAN_FLAG_TXRX_RUN, osWaitForever, false);

        if (txrx_flags.get() & CFE_CAN_FLAG_TXRX_SHUTDOWN) {
            // clear out the tx buffers
            while (txrx_try_write_hs());
            while (txrx_try_write_ls());

            CAN_hs->mode(CAN::Silent);
            CAN_ls->mode(CAN::Silent);

            txrx_flags.set(CFE_CAN_FLAG_MAIN_TXRX_CLEAN);
            txrx_flags.clear(CFE_CAN_FLAG_TXRX_SHUTDOWN | CFE_CAN_FLAG_TXRX_RUN);
        } else {
            txrx_try_read_hs();
            txrx_try_read_ls();
            txrx_try_write_hs();
            txrx_try_write_ls();
        }
    }
}

int CANbus::txrx_try_read_ls() {
    int ret = CAN_ls->read(rx_frame);

    if (ret) {
        // TODO: check if we can write to the queue
        CANPacket *packet = rx_queue.try_alloc();
        packet->id = rx_frame.id;
        memcpy(&packet->data[0], &rx_frame.data[0], 8 * sizeof(uint8_t));
        rx_queue.put(packet);
    }

    return ret;
}

int CANbus::txrx_try_read_hs() {
    int ret = CAN_hs->read(rx_frame);

    if (ret) {
        // TODO: check if we can write to the queue
        CANPacket *packet = rx_queue.try_alloc();
        packet->id = rx_frame.id;
        memcpy(&packet->data[0], &rx_frame.data[0], 8 * sizeof(uint8_t));
        rx_queue.put(packet);
    }

    return ret;
}

bool CANbus::txrx_try_write_ls() {
    // check if we can write frames
    bool ret = !tx_queue_ls.empty();

    if (ret) {
        CANPacket *packet = tx_queue_ls.try_get();
        tx_ls(packet);
    }

    return ret;
}

bool CANbus::txrx_try_write_hs() {
    // check if we can write frames
    bool ret = !tx_queue_hs.empty();

    if (ret) {
        CANPacket *packet = tx_queue_hs.try_get();
        tx_hs(packet);
    }

    return ret;
}

void CANbus::tx_build_message(CANPacket *packet) {
    tx_frame.format = CANFormat::CANExtended;
    tx_frame.id = packet->id;
    tx_frame.len = 8;
    memcpy(tx_frame.data, packet->data, sizeof(tx_frame.data));
}

