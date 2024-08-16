// mbed rtos events example

#include <mbed.h>
#include <mbed_events.h>

#include <VolvoLib.h>

#include "canbus/canbus.h"
#include "listeners/swm_inputs/swm_inputs.h"
#include "listeners/daylight_sensor/daylight_sensor.h"

#include "manager/power_state_manager.h"

EventQueue equeue;
void handler(int count);
Event<void(int)> event(&equeue, handler);

void handler(int count) {
    printf("Event = %d \n", count);
    return;
}

void post_events(void) {
    event.post(1);
    event.post(2);
    event.post(3);
}

int main() {
    // Initialize CAN bus
    CANbus *can = CANbus::getInstance();

    // Initialize CAN bus listeners
    SWM_Inputs *swm_inputs = SWM_Inputs::getInstance();
    DaylightSensor *daylight_sensor = DaylightSensor::getInstance();

    // Add CAN bus listeners to CAN bus
    can->addListener(swm_inputs);
    can->addListener(daylight_sensor);

    // Create an instance of the power manager
    PowerStateManager* psmgr = PowerStateManager::getInstance();

    // start the power manager on the main thread.
    psmgr->start();
}
