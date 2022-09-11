#include "event_loop.h"
#include "subreactor.h"

int main() {
    event_loop loop;
    loop.process_event();
    return 0;
}