#ifndef __POWER_STATE_MANAGER_H
#define __POWER_STATE_MANAGER_H

#include <mbed.h>
#include "console/logging.h"
#include "subsystem.h"
#include "console/console.h"

#define CFE_PSM_FLAG_MAIN_WAKEUP (1UL << 1) // Allows the main thread loop to run.
#define CFE_PSM_FLAG_MAIN_SHUTDOWN (1UL << 2) // Signals the main thread to begin shutting down.
#define CFE_PSM_FLAG_SUB_ENABLE (1UL << 3) // Allows the subsystem thread loop to run.
#define CFE_PSM_FLAG_MAIN_SUB_CLEAN (1UL << 4) // Signals the main thread that the subsystem has shut down.
#define CFE_PSM_FLAG_SUB_SHUTDOWN (1UL << 5) // Signals the subsystem thread to begin shutting down.

/**
* Controls the system power state. Allows OpenCFE to enter low power sleep mode
* when car turns off, preventing battery drain. Uses hardware interrupts to wake
* up automatically.
*/
class PowerStateManager {
public:
    static PowerStateManager* getInstance() {
        static PowerStateManager instance;
        return &instance;
    }

    /**
    * Signals between the PSM, Subsystem, and Hardware to control the wake-up
    * and shutdown process.
    */
    EventFlags* flags;

    // Start the power management system. This function does not return.
    void start();

    // Tell the power state manager to begin the wake-up process.
    void requestWakeup();

    /**
    * Tell the subsystem thread to begin shutting down.
    * Note that this function returns immediately while the system may take a
    * few seconds to complete the shutdown process.
    */
    void requestShutdown();

    // Prevent the power state manager from shutting down.
    void caffeinate();

    // Allow the power state manager to shut down.
    void decaffeinate();

    // Check if shutting down is prevented.
    bool isCaffeinated();

    /**
    * The Interrupt Service Routine (ISR) that responds to the CAN wakeup
    * interrupt.
    */
    void canWakeupISR();
protected:
    /**
    * The function executed by the subsystem thread.
    */
    void _subsystemTask();
private:
    PowerStateManager();

    /**
    * Hardware interrupt in the CAN_LS RX pin so that we are woken up by CAN bus
    * activity.
    */
    InterruptIn* _canWakeupInterrupt;

    /**
    * Thread that manages the subsystem lifecycle and executes events posted by
    * subsystem modules.
    */
    Thread* _subsystemThread;

    /**
    * OpenCFE's Module Manager. Instantiates the modules and calls their
    * start/stop functions.
    */
    Subsystem* _subsystem;

    /**
    * The OpenCFE master event queue. All events should be posted here. By
    * default, this is set to mbed_event_queue().
    */
    EventQueue* _systemEventQueue;

    /**
    * Status LED to indicate when the PSM is enabled.
    */
    DigitalOut* _psmLED;

    /**
    * Simple UART CLI for testing and debug.
    */
    Console* _console;

    Logging* _log;

    /**
    * True when the PSM is prevented from shutting down.
    */
    bool _caffeinated;
};

#endif
