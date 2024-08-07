#include "comms/listeners/swm_inputs/swm_inputs.h"
#include <cstdint>

SWM_Inputs::SWM_Inputs() {
    // TODO: Should we set a bitmask here?
    canbus->addListener(this);
}

// TODO: move this to helper class
bool get_bit(uint8_t byte, int bit) {
    return 1 == ( (byte >> bit) & 1);
}

void SWM_Inputs::listener_update(CANPacket *packet) {
    switch (packet->id) {
        case SWM_MESSAGE_1: {
            // Media Controls
            media_controls = {};

            // All of these need to be inverted for some reason...
            const uint8_t d7 = ~packet->data[7];

            media_controls.prev = get_bit(d7, 0);
            media_controls.next = get_bit(d7, 1);
            media_controls.vol_up = get_bit(d7, 2);
            media_controls.vol_down = get_bit(d7, 3);

            // RTI Controls
            rti_controls = {};

            const uint8_t d6 = packet->data[6];

            rti_controls.right = get_bit(d6, 0);
            rti_controls.left = get_bit(d6, 1);
            rti_controls.down = get_bit(d6, 2);
            rti_controls.up = get_bit(d6, 3);
            rti_controls.back = get_bit(d6, 4);
            rti_controls.enter = get_bit(d6, 5);

            break;
        }
        case SWM_MESSAGE_2: {
            break;
        }
        default: {
            break;
        }
    }
}
