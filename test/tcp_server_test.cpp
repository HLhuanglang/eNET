#include "event_loop.h"
#include "tcp_connection.h"
#include "tcp_server.h"

int main()
{
    event_loop loop;
    tcp_server server(&loop, "0.0.0.0", 1234);
}