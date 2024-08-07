#include "comms/listeners/daylight_sensor/daylight_sensor.h"

DaylightSensor::DaylightSensor() {
    listener_params.mask = 0xFFFFFFFF;
    canbus->addListener(this);
}

void DaylightSensor::listener_update(CANPacket *packet) {
    const int daylightMessage = DAYLIGHT_MESSAGE;
    if (packet->id == daylightMessage) {
        daylight = packet->data[7] - 0x10;
    }
}
