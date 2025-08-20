#ifndef __CANBUS_H
#define __CANBUS_H

#include <mbed.h>
#include "../subsystem.h"

#define CAN_RX_QUEUE_SIZE 50
#define CAN_RX_QUEUE_MESSAGES_PER_PERIOD 25
#define CAN_TX_QUEUE_SIZE 25

#define CFE_CAN_FLAG_TXRX_RUN (1UL << 1)
#define CFE_CAN_FLAG_MAIN_TXRX_CLEAN (1UL << 2)
#define CFE_CAN_FLAG_TXRX_SHUTDOWN (1UL << 3)

struct CANPacket {
    uint32_t id;
    uint8_t data[8] = {0};
};

class CANbus : public Subsystem::Module {
public:
    typedef enum Bus {LS, HS} Bus;
    class Listener {
        public:
            virtual void listener_update(CANPacket *packet);
    };

    static CANbus* getInstance() {
        static CANbus instance;
        return &instance;
    };

    void tx_q_ls(CANPacket *packet);
    void tx_q_hs(CANPacket *packet);

    void notify_listeners(CANPacket *packet);
    void addListener(CANbus::Listener *listener);

    void start();
    void stop();

    void addRxFilter(Bus bus, uint32_t id, uint32_t mask);
protected:
    EventFlags txrx_flags;

private:
    CANbus();
    CAN* CAN_ls;
    CAN* CAN_hs;

    Subsystem* subsystem;

    DigitalOut* canLED;

    void periodic();
    Event<void()>* periodicEvent;

    void tx_ls(CANPacket *packet);
    void tx_hs(CANPacket *packet);

    std::vector<CANbus::Listener*> rx_listeners;
    Mail<CANPacket, CAN_RX_QUEUE_SIZE> rx_queue;
    Mail<CANPacket, CAN_TX_QUEUE_SIZE> tx_queue_ls;
    Mail<CANPacket, CAN_TX_QUEUE_SIZE> tx_queue_hs;
    uint32_t last_can_rx = 0;

    bool allow_queue_tx = false;

    CANMessage tx_frame;
    CANMessage rx_frame;

    Thread txrx_processor;
    void txrx_loop();
    int txrx_try_read_hs();
    int txrx_try_read_ls();
    bool txrx_try_write_ls();
    bool txrx_try_write_hs();

    void tx_build_message(CANPacket *packet);
};

#endif
