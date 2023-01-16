#ifndef __EASYNET_CHANNEL_H
#define __EASYNET_CHANNEL_H

#include "cb.h"

class event_loop;

class channel {
public:
    channel(event_loop *loop, int fd);

private:
    const int fd_;
};

#endif