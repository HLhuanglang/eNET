#include "ev_thread.h"
#include "event_loop.h"

int main() {
    event_loop loop;
    loop.process_event();
    return 0;
}