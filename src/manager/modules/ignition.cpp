#include "ignition.h"
#include "manager/power_state_manager.h"

Ignition::Ignition() {
    subsystem = Subsystem::getInstance();
    canbus = CANbus::getInstance();
    canbus->addListener(this);
    canbus->addRxFilter(CANbus::Bus::LS, CFE_CAN_IGNITION_MSG_ID, ((1<<29)-1));
    switchPos = KeyOut;
    EventQueue *queue = mbed_event_queue();
    periodicEvent = new Event<void()>(queue, callback(this, &Ignition::periodic));
    periodicEvent->period(50ms);

    keyOutSleepEvent = new Event<void()>(queue, callback(this, &Ignition::keyOutSleep));
    keyOutSleepEvent->delay(15s); // dispatch sleep event 15s after post
}

void Ignition::listener_update(CANPacket *packet) {
    if (packet->id == CFE_CAN_IGNITION_MSG_ID) {
        uint8_t ignSwitchData = packet->data[6];

        // match key position bitfield in descending order
        if (ignSwitchData & CFE_CAN_IGNITION_FILTER_KPIII) {
            switchPos = KPIII;
        } else if (ignSwitchData & CFE_CAN_IGNITION_FILTER_KPII) {
            switchPos = KPII;
        } else if (ignSwitchData & CFE_CAN_IGNITION_FILTER_KPI) {
            switchPos = KPI;
        } else if (ignSwitchData & CFE_CAN_IGNITION_FILTER_KP0) {
            switchPos = KP0;
        } else {
            switchPos = KeyOut;
        }
    }
}

void Ignition::start() {
    periodicEvent->post();
}

void Ignition::stop() {
    periodicEvent->cancel();
}

void Ignition::periodic() {
    if (getKeyPos() != KeyOut) {
        if (!sleepEventCanceled) {
            keyOutSleepEvent->cancel();
            sleepEventCanceled = true;
            sleepEventPosted = false;
        }
    } else {
        if (!sleepEventPosted) {
            keyOutSleepEvent->post();
            sleepEventCanceled = false;
            sleepEventPosted = true;
        }
    }
}

void Ignition::keyOutSleep() {
    PowerStateManager::getInstance()->requestFullSystemShutdown();
}
