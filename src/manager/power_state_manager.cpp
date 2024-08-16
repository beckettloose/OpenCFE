#include "power_state_manager.h"

PowerStateManager::PowerStateManager() {
    flags = new EventFlags;

    // The low speed bus always goes active before the high speed bus,
    // so we only need to set up one interrupt.
    _canWakeupInterrupt = new InterruptIn(PD_1); // this is the CAN_LS rx pin

    _subsystemThread = new Thread;
    _systemEventQueue = mbed_event_queue();

    _subsystem = Subsystem::getInstance();
}

void PowerStateManager::start() {
    // Start the subsystem thread (it will block instantly, though)
    _subsystemThread->start(callback(this, &PowerStateManager::_subsystemTask));

    // Wait until our first enable signal (this will come from the first CAN frame)
    flags->wait_all(CFE_PSM_FLAG_MAIN_WAKEUP, osWaitForever, false);

    // Enable the subsystem
    requestSubsystemStartup();

    // enter an infinite loop
    while (true) {
        // block until the enable flag is set
        flags->wait_all(CFE_PSM_FLAG_MAIN_WAKEUP, osWaitForever, false);

        // TODO: determine if we should start the subsystem thread automatically
    }
}

void PowerStateManager::requestSubsystemShutdown() {
    // Set the subsystem shutdown flag
    flags->set(CFE_PSM_FLAG_SUB_SHUTDOWN);
}

void PowerStateManager::requestSubsystemStartup() {
    // If the main thread is enabled
    if (flags->get() & CFE_PSM_FLAG_MAIN_WAKEUP) {
        // set the subsystem enable flag
        flags->set(CFE_PSM_FLAG_SUB_ENABLE);
    }
}

void PowerStateManager::requestFullSystemStartup() {
    // set the main wakeup flag
    flags->set(CFE_PSM_FLAG_MAIN_WAKEUP);
}

void PowerStateManager::requestFullSystemShutdown() {
}

void PowerStateManager::canWakeupISR() {
    // Immediately disable the CAN wakeup IRQ since we would otherwise get
    // around 125,000 interrupts/sec.
    _canWakeupInterrupt->disable_irq();

    // Set the main wakeup flag to enable everything
    flags->set(CFE_PSM_FLAG_MAIN_WAKEUP);
}

void PowerStateManager::_subsystemTask() {
    bool shouldSetCleanFlag = false;

    while (true) {
        flags->wait_all(CFE_PSM_FLAG_SUB_ENABLE, osWaitForever, false);

        flags->clear(CFE_PSM_FLAG_MAIN_SUB_CLEAN);

        if (flags->get() & CFE_PSM_FLAG_SUB_SHUTDOWN) {
            _subsystem->stop();

            if (_subsystem->getState() == Subsystem::State::STOPPED) {
                shouldSetCleanFlag = true;
            }
        } else {
            _subsystem->start();
        }

        _systemEventQueue->dispatch_for(1000ms);

        if (shouldSetCleanFlag) {
            shouldSetCleanFlag = false;
            flags->set(CFE_PSM_FLAG_MAIN_SUB_CLEAN);
            flags->clear(CFE_PSM_FLAG_SUB_ENABLE | CFE_PSM_FLAG_SUB_SHUTDOWN);
        }
    }
}
