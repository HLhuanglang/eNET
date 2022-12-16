#include "subreactor_pool.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <csignal>

void handle_sig(int)
{
    std::cout << "bye~" << std::endl;
    exit(0);
}

void func(event_loop *loop, void *args)
{
    char *str = (char *)args;
    std::cout << str << std::endl;
}

int main()
{
    signal(SIGINT, handle_sig);
    subreactor_pool pool(4);
    while (1) {
        auto reactor = pool.get_sub_reactor();
        task_t t;
        t.func_    = func;
        char str[] = "hello,world";
        t.args_    = (void *)(str);

        msg_t msg;
        msg.msg_type_ = msg_type_t::NEW_TASK;
        msg.task_     = &t;
        reactor->notify(msg);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}