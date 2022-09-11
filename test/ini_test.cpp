#include "../util/ini.h"
#include <iostream>

int main() {
    ini i;
    ini_handle handle("./test.ini");
    handle.read(i);
    auto ret = i.get_val("log", "log_name");
    std::cout << ret << std::endl;
}