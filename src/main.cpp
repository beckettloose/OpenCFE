// mbed rtos events example

#include "mbed.h"

EventQueue queue;
void handler(int count);
Event<void(int)> event(&queue, handler);

void handler(int count) {
    printf("Event = %d \n", count);
    return;
}

void post_events(void) {
    event.post(1);
    event.post(2);
    event.post(3);
}

int main() {
    Thread event_thread;

    event.delay(100);
    event.period(200);

    event_thread.start(callback(post_events));

    queue.dispatch(400);

    event_thread.join();
}
