#ifndef __SUBSYSTEM_H
#define __SUBSYSTEM_H

#include "libraries/real_semaphore.h"
#include <mbed.h>
#include <vector>

// Manages the modules that make up the CFE's core and additional functions.
class Subsystem {
public:
    // An independent unit that is responsible for a single group of actions.
    // Provides functions that are called at startup and shutdown.
    //
    // Modules must use the provided registry to report their status.
    // (register at startup, deregister when finished)
    //
    // Modules must use the system event queue to schedule any additional
    // functions for execution.
    class Module {
    public:
        virtual void start();
        virtual void stop();
    };

    static Subsystem* getInstance() {
        static Subsystem instance;
        return &instance;
    }

    typedef enum State {
        STOPPED,
        STOPPING,
        STARTING,
        STARTED
    } State;

    // keeps track of the number of active modules.
    RealSemaphore* registry;

    void init();
    bool start();
    bool stop();

    virtual State getState() { return state; }

private:
    Subsystem();
    EventQueue* systemQueue;

    // order-dependent synchronusly controlled modules.
    std::vector<Subsystem::Module*>* modules;

    State state = STOPPED;

    // internal functions

    void _startModules();
    void _stopModules();
};

#endif
