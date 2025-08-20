#include <mbed.h>
#include <mbed_events.h>

#include "opencfe/power_state_manager.h"

int main() {
    PowerStateManager* psmgr = PowerStateManager::getInstance();
    psmgr->start();
}
