#ifndef __IGNITION_H
#define __IGNITION_H

#include "canbus.h"
#include "../subsystem.h"

// TODO: Figure out this message ID
#define CFE_CAN_IGNITION_MSG_ID 0x0

#define CFE_CAN_IGNITION_FILTER_KPIII 0b00000000
#define CFE_CAN_IGNITION_FILTER_KPII 0b00000000
#define CFE_CAN_IGNITION_FILTER_KPI 0b00000000
#define CFE_CAN_IGNITION_FILTER_KP0 0b00000000

class Ignition : public Subsystem::Module, public CANbus::Listener {
public:
    enum KeyPos {KeyOut, KP0, KPI, KPII, KPIII};
    static Ignition* getInstance() {
        static Ignition instance;
        return &instance;
    }

    void listener_update(CANPacket *packet);

    void start();
    void stop();

    // Get the current key position in standard Volvo format.
    //
    // Key out = KeyOut
    // Key in, not turned = KP0
    // Key in, accessory mode = KPI
    // Key in, run mode = KPII
    // Key in, starter mode = KPIII
    virtual KeyPos getKeyPos() { return switchPos; }
private:
    Ignition();
    Subsystem* subsystem;
    CANbus* canbus;
    KeyPos switchPos;

    Event<void()>* periodicEvent;
    void periodic();

    Event<void()>* keyOutSleepEvent;
    void keyOutSleep();
    bool sleepEventPosted = false;
    bool sleepEventCanceled = false;
    bool _cafState = false;
    bool _lastCafState = false;
};

#endif
