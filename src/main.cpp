// mbed rtos events example

#include <mbed.h>
#include <mbed_events.h>

#include <VolvoLib.h>

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

    // Request the shared event queue
    EventQueue *queue = mbed_event_queue();

    // Dispatch the shared queue forever
    queue->dispatch_forever();
}
