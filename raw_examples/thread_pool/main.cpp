#include <unistd.h>

#include <string>
#include <thread>

#include "msg.h"
#include "thread_pool.h"

int main() {
    thread_pool pool(100, 2, nullptr, nullptr);

    for (int i = 0; i < 10; i++) {
        sleep(5);
        test_msg msg;
        msg.set_msg("hello,world! " + std::to_string(i));
        pool.commit(msg);  // 主线程投递信息,工作线程应该被唤醒才对
        std::cout << msg << std::endl;
    }
}