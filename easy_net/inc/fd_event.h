#ifndef __EASYNET_FD_EVENT_H
#define __EASYNET_FD_EVENT_H

#include "non_copyable.h"
#ifdef __linux__
#    include <sys/poll.h>
#endif

const int k_non_event = 0;
const int k_read_event = POLLIN | POLLPRI;
const int k_write_evnet = POLLOUT;

#endif