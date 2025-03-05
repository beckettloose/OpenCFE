#include "subsystem.h"
#include "manager/modules/canbus.h"
#include "manager/modules/ignition.h"
#include "manager/modules/lcd.h"
#include "manager/modules/lcd_welcome_msg.h"
#include "mbed_shared_queues.h"
#include <algorithm>
#include <vector>

Subsystem::Subsystem() {
    systemQueue = mbed_event_queue();
    modules = new std::vector<Module*>;
    registry = new RealSemaphore;
}

void Subsystem::init() {
    // This defines the load and unload order of the modules.
    modules->push_back(CANbus::getInstance());
    modules->push_back(Volvo::LCD::getInstance());
    modules->push_back(Ignition::getInstance());
    modules->push_back(LCDWelcomeMessage::getInstance());
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
