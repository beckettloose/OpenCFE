#ifndef SWM_INPUTS_H
#define SWM_INPUTS_H

#include "comms/canbus/canbus.h"
#include <cstdint>

#define SWM_MESSAGE_1 0x00404066
#define SWM_MESSAGE_2 0x0221300A

/*#define MEDIA_PREV 0x01*/
/*#define MEDIA_NEXT 0x02*/
/*#define MEDIA_VOL_UP 0x04*/
/*#define MEDIA_VOL_DOWN 0x08*/

class SWM_Inputs : public CANbus::Listener {
public:
    struct RTIControls {
        bool right;
        bool left;
        bool down;
        bool up;
        bool back;
        bool enter;
    };

    struct MediaControls {
        bool prev;
        bool next;
        bool vol_up;
        bool vol_down;
    };

    // TODO: Verify the order of bits
    struct CruiseControls {
        bool on_off;
        bool accel;
        bool decel;
        bool resume;
        bool cancel;
    };

    // TODO: Verify the order of bits
    struct TripControls {
        bool read;
        bool reset;
    };

    static SWM_Inputs *getInstance() {
        static SWM_Inputs instance;
        return &instance;
    }

    virtual RTIControls getRTIControls() {
        return rti_controls;
    }
    virtual MediaControls getMediaControls() {
        return media_controls;
    }
    virtual CruiseControls getCruiseControls() {
        return cruise_controls;
    }
    virtual TripControls getTripControls() {
        return trip_controls;
    }
    virtual bool getRotaryInitialized() {
        return rotary_switch_initialized;
    }
    virtual uint8_t getRotaryPosition() {
        return rotary_switch_position;
    }

private:
    SWM_Inputs();
    CANbus *canbus = CANbus::getInstance();
    void listener_update(CANPacket *packet);

    RTIControls rti_controls = {};
    MediaControls media_controls = {};
    CruiseControls cruise_controls = {};
    TripControls trip_controls = {};
    uint8_t rotary_switch_position = 0;
    bool rotary_switch_initialized = false;
};

#endif
