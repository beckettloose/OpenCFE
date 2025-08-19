#include <mbed.h>
#include <mbed_events.h>

#include "opencfe/power_state_manager.h"

int main() {
    // Create an instance of the power manager
    PowerStateManager* psmgr = PowerStateManager::getInstance();

    // start the power manager on the main thread.
    psmgr->start();
}
