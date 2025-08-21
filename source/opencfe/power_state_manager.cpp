#include "power_state_manager.h"

PowerStateManager::PowerStateManager() {
    flags = new EventFlags;
    flags->clear();

    // The low speed bus always goes active before the high speed bus,
    // so we only need to set up one interrupt.
    _canWakeupInterrupt = new InterruptIn(PD_0); // this is the CAN_LS rx pin
    _canWakeupInterrupt->fall(callback(this, &PowerStateManager::canWakeupISR));

    _subsystemThread = new Thread;
    _systemEventQueue = mbed_event_queue();
    _subsystem = Subsystem::getInstance();

    _psmLED = new DigitalOut(PB_0);

    _console = Console::getInstance();
    _log = Logging::getInstance();

    _caffeinated = false;
}

void PowerStateManager::start() {
    _log->debug("PSM", "Starting...");
    _subsystem->init();
    _subsystemThread->start(callback(this, &PowerStateManager::_subsystemTask));
    _console->init();

    _log->debug("PSM", "Init complete, waiting for wakeup signal");
    while (true) {
        flags->wait_all(CFE_PSM_FLAG_MAIN_WAKEUP, osWaitForever, false);
        flags->set(CFE_PSM_FLAG_SUB_ENABLE);
        flags->clear(CFE_PSM_FLAG_MAIN_WAKEUP);
        _psmLED->write(1);
    }
}

void PowerStateManager::requestWakeup() {
    // TODO: fix mutex issue in logger becase wakeup calls this from an ISR

    // _log->debug("PSM", "Got wake-up request");
    flags->set(CFE_PSM_FLAG_MAIN_WAKEUP);
}

void PowerStateManager::requestShutdown() {
    if (!_caffeinated) {
        _log->debug("PSM", "Got shutdown request");
        flags->set(CFE_PSM_FLAG_SUB_SHUTDOWN);
    }
}

void PowerStateManager::caffeinate() {
    _log->info("PSM", "Caffeinating...");
    _caffeinated = true;
}

void PowerStateManager::decaffeinate() {
    _log->info("PSM", "Decaffeinating...");
    _caffeinated = false;
}

bool PowerStateManager::isCaffeinated() {
    return _caffeinated;
}

void PowerStateManager::canWakeupISR() {
    _canWakeupInterrupt->disable_irq();
    requestWakeup();
}

void PowerStateManager::_subsystemTask() {
    bool shouldSetCleanFlag = false;

    while (true) {
        if (!(flags->get() & CFE_PSM_FLAG_SUB_ENABLE)) {
            _log->debug("PSM", "Subsystem waiting for enable signal");
        }
        flags->wait_all(CFE_PSM_FLAG_SUB_ENABLE, osWaitForever, false);

        flags->clear(CFE_PSM_FLAG_MAIN_SUB_CLEAN);

        if (flags->get() & CFE_PSM_FLAG_SUB_SHUTDOWN) {
            _log->debug("PSM", "Subsystem received shutdown signal");
            _subsystem->stop();
            _console->stop();

            if (_subsystem->getState() == Subsystem::State::STOPPED) {
                shouldSetCleanFlag = true;
            }
        } else {
            _subsystem->start();
            _console->start();
        }

        _systemEventQueue->dispatch_for(1000ms);

        if (shouldSetCleanFlag) {
            shouldSetCleanFlag = false;
            flags->set(CFE_PSM_FLAG_MAIN_SUB_CLEAN);
            flags->clear(CFE_PSM_FLAG_SUB_ENABLE | CFE_PSM_FLAG_SUB_SHUTDOWN);
            _log->debug("PSM", "Subsystem finished shutting down");
            _canWakeupInterrupt->enable_irq();
            _psmLED->write(0);
        }
    }
}
