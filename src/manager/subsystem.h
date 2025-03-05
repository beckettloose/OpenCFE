#ifndef __SUBSYSTEM_H
#define __SUBSYSTEM_H

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

    void init();
    bool start();
    bool stop();

    virtual State getState() { return state; }

private:
    Subsystem();
    EventQueue* systemQueue;

    DigitalOut* _subsystemLED;

    // order-dependent synchronusly controlled modules.
    std::vector<Subsystem::Module*>* modules;

    State state = STOPPED;

    // internal functions

    void _startModules();
    void _stopModules();
};

#endif
