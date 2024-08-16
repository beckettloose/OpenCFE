#include "real_semaphore.h"

void RealSemaphore::acquire() {
    used++;
}

void RealSemaphore::release() {
    used--;
}

uint RealSemaphore::numUsed() {
    return used;
}

bool RealSemaphore::empty() {
    return used == 0;
}
