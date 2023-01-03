#include "easy_net/event_loop.h"
#include "easy_net/tcp_connection.h"
#include "easy_net/tcp_server.h"

int main()
{
    event_loop loop;
    tcp_server server(&loop, "0.0.0.0", 1234, 4);
}