#ifndef __CB_H
#define __CB_H

#include <functional>

// Pre-declaration
class event_loop;
using event_cb_f = std::function<void(event_loop *loop, int fd, void *args)>;
using task_cb_f = std::function<void(event_loop *loop, void *args)>;

#endif