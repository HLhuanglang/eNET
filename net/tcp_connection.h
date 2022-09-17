#ifndef __TCP_CONNECTION_H
#define __TCP_CONNECTION_H

#include <cstddef>
class tcp_connection {
public:
    int send_data(const char* data, size_t data_size);
};

#endif