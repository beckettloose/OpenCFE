// mbed rtos events example

#include <mbed.h>
#include <mbed_events.h>

#include <VolvoLib.h>

#include "manager/power_state_manager.h"

int main() {
    printf("\n\n\nStarting OpenCFE Development Build...\n");

    // Create an instance of the power manager
    PowerStateManager* psmgr = PowerStateManager::getInstance();

    // start the power manager on the main thread.
    psmgr->start();
}
