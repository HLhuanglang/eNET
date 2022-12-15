#include "buffer.h"
#include <string>

int main()
{
    buffer* buf     = new buffer;
    std::string str = "hello,world";
    buf->append(str.c_str(), str.size());
    return 0;
}