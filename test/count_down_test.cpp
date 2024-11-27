#include <gtest/gtest.h>

#include <thread>

#include "countdown_latch.h"

class Job {
 public:
    Job(std::string name) : name{std::move(name)} {}

    const std::string name;
    std::string product{"未工作"};
    std::thread action{};
};

TEST(CountDown, BasicTest) {
    Job jobs[]{{"Annika"}, {"Buru"}, {"Chuck"}};

    EasyNet::CountDownLatch work_done{sizeof(jobs) / sizeof(jobs[0])};
    EasyNet::CountDownLatch start_clean_up{1};

    auto work = [&](Job& my_job) {
        my_job.product = my_job.name + " 已工作";
        work_done.CountDown();
        start_clean_up.Wait();
        my_job.product = my_job.name + " 已清理";
    };

    std::string actual;
    actual += "工作启动... ";
    for (auto& job : jobs)
        job.action = std::thread{work, std::ref(job)};

    work_done.Wait();
    actual += "完成:\n";
    for (auto const& job : jobs)
        actual += " " + job.product + '\n';

    actual += "清理工作线程... ";
    start_clean_up.CountDown();
    for (auto& job : jobs)
        job.action.join();

    actual += "完成:\n";
    for (auto const& job : jobs)
        actual += " " + job.product + '\n';

    std::string excepted = "工作启动... 完成:\n Annika 已工作\n Buru 已工作\n Chuck 已工作\n清理工作线程... 完成:\n Annika 已清理\n Buru 已清理\n Chuck 已清理\n";
    EXPECT_EQ(actual, excepted);
}