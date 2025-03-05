#ifndef __POWER_STATE_MANAGER_H
#define __POWER_STATE_MANAGER_H

#include <mbed.h>
#include "DigitalOut.h"
#include "manager/subsystem.h"

#define CFE_PSM_FLAG_MAIN_WAKEUP (1UL << 1) // Allows the main thread loop to run.
#define CFE_PSM_FLAG_MAIN_SHUTDOWN (1UL << 2) // Signals the main thread to begin shutting down.
#define CFE_PSM_FLAG_SUB_ENABLE (1UL << 3) // Allows the subsystem thread loop to run.
#define CFE_PSM_FLAG_MAIN_SUB_CLEAN (1UL << 4) // Signals the main thread that the subsystem has shut down.
#define CFE_PSM_FLAG_SUB_SHUTDOWN (1UL << 5) // Signals the subsystem thread to begin shutting down.

// Controls the system power state
class PowerStateManager {
public:
    static PowerStateManager* getInstance() {
        static PowerStateManager instance;
        return &instance;
    }

    // Signal between manager and subsystem to make sure we finish up before
    // entering deep sleep. Also lets us block the main thread until our wakeup
    // ISR is triggered.
    EventFlags* flags;

    // Start the power management system. This function does not return.
    void start();

    // Tell the subsystem thread to begin shutting down.
    // Note that this does not happen instantly, so the caller must assume
    // that the system may continue to run for a period of time after this
    // is called.
    void requestSubsystemShutdown();

    // Tell the subsystem thread to start up.
    void requestSubsystemStartup();

    // Tell the main thread to start running.
    void requestFullSystemStartup();

    // Tell the main thread to try shutting down.
    void requestFullSystemShutdown();

    // The Interrupt Service Routine that responds to the CAN wakeup interrupt.
    void canWakeupISR();
protected:
    void _subsystemTask();
private:
    PowerStateManager();

    // Allows us to use the CAN RX pin to wake up from deep sleep
    InterruptIn* _canWakeupInterrupt;

    // Gives us direct control over the subsystem thread
    Thread* _subsystemThread;

    Subsystem* _subsystem;

    EventQueue* _systemEventQueue;

    DigitalOut* _psmLED;
};

#endif
