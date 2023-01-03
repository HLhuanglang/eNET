#include "httplib.h"

using namespace httplib;

int main()
{
    httplib::Client cli("localhost", 12345);
    httplib::Client cli2("xxxxxx");
    httplib::Headers headers{
        {"Accept-Encoding", "gzip, deflate"},
        {"Holo_wo", "hhhhh"}};
    std::string body;
    if (auto res = cli.Get("/hi", headers)) {
        if (res->status == 200) {
            std::cout << res->body << std::endl;
        }
    } else {
        auto err = res.error();
        std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
    }
}