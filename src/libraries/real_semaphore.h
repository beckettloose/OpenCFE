#ifndef __REAL_SEMAPHORE_H
#define __REAL_SEMAPHORE_H

#include <mbed.h>

// "If computer science trees are upside-down from real trees, then a real
// semaphore must be upside-down from a compsci semaphore."
//
// Implements an unlimited resource utilization tracker for the purpose of
// signaling to a parent controller whether or not their children are active.
//
// In this case, it is used to prevent us from entering deep sleep until all
// subsystem modules have cleaned up safely, sort of like a busy indicator.
class RealSemaphore {
    uint used;
public:
    void acquire();
    void release();
    uint numUsed();
    bool empty();
};

#endif
