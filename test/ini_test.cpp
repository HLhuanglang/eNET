#include <cstring>
#include <iostream>

#include "easy_net/ini.h"

int main() {
    inifile ifile;
    if (ifile.load("test.ini")) {
        ini::read(ifile);

        auto ret = ifile.get_val("log", "log_name");
        std::cout << ret << std::endl;

        ifile.set_val("log", "log_name", "123");
        ini::write(ifile);
    } else {
        std::cout << "load test.ini error: " << strerror(errno) << std::endl;
    }
    return 0;
}