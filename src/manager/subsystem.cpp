#include "subsystem.h"
#include "mbed_shared_queues.h"
#include <algorithm>
#include <vector>

Subsystem::Subsystem() {
    systemQueue = mbed_event_queue();
    modules = new std::vector<Module*>;
    registry = new RealSemaphore;
}

bool Subsystem::start() {
    if (state == STOPPED) {
        state = STARTING;
        systemQueue->call(this, &Subsystem::_startModules);
        return true;
    }
    return false;
}

bool Subsystem::stop() {
    if (state == STARTED) {
        state = STOPPING;
        systemQueue->call(this, &Subsystem::_stopModules);
        return true;
    }
    return false;
}

void Subsystem::_startModules() {
    std::for_each(
        modules->begin(),
        modules->end(),
        [](const auto & module){
            module->start();
        }
    );
    state = STARTED;
}

void Subsystem::_stopModules() {
    std::for_each(
        modules->end(),
        modules->begin(),
        [](const auto & module){
            module->stop();
        }
    );
    state = STOPPED;
}
