#ifndef __DAYLIGHT_SENSOR_H
#define __DAYLIGHT_SENSOR_H

#include "comms/canbus/canbus.h"

#define DAYLIGHT_MESSAGE 0x2802262

class DaylightSensor : public CANbus::Listener {
public:
    static DaylightSensor *getInstance() {
        static DaylightSensor instance;
        return &instance;
    }
    void listener_update(CANPacket *packet);
    virtual int getDaylight() {
        return daylight;
    }

private:
    DaylightSensor();
    CANbus *canbus = CANbus::getInstance();

    int daylight = 0xF; // default to full brightness
};

#endif
