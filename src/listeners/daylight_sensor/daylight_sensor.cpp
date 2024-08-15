#include "daylight_sensor.h"

namespace Volvo {

DaylightSensor::DaylightSensor() {
    canbus->addListener(this);
}

void DaylightSensor::listener_update(CANPacket *packet) {
    const int daylightMessage = DAYLIGHT_MESSAGE;
    if (packet->id == daylightMessage) {
        daylight = packet->data[7] - 0x10;
    }
}

}
