#ifndef __DEF_H
#define __DEF_H

const int k_timeouts_ms = 10 * 1000;  // epoll_wait超时时间1s
const int k_init_eventlist_size = 16; // epoll_wait初始化时监听是的事件数量
const int k_sub_reactor_cnt = 1;      // sub reactor数量

#endif