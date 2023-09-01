#include <iostream>
#include <thread>
#include <unistd.h>

#include "easy_net/event_loop.h"
#include "easy_net/notify.h"

int main() {
    event_loop loop;

    loop.run_in_loop([]() {
        std::cout << "hello,world" << std::endl;
    });

    std::thread t([&loop]() {
        sleep(5);
        loop.wakeup();
    });
    t.detach();

    loop.loop();
    return 0;
}