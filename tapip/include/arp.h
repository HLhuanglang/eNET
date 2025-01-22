#ifndef __ARP_H
#define __ARP_H

#include "ether.h"
#include "list.h"
#include "netif.h"

#define ARP_ETHERNET
#define ARP_IP

/* arp cache */
#define ARP_CACHE_SZ 20
#define ARP_TIMEOUT 600 /* 10 minutes */
#define ARP_WAITTIME 1

/* arp entry state */
// FAQ：这里的状态有什么用？
// A：为了明确当前这个表项是什么状态,不同状态下的处理方式是不一样的.
// 1. ARP_FREE: 表示这个表项是空闲的,可以被分配使用.
// 2. ARP_WAITING: 表示这个表项是等待状态,可能是在等待对方的回复,也可能是在等待重试.
// 3. ARP_RESOLVED: 表示这个表项是已经解析完成的,可以直接使用.
// 从lwip的实现看,可以做一个更加复杂的状态机
/*
ETHARP_STATE_EMPTY：空状态。当前entry资源无效，可以被填充使用。
ETHARP_STATE_PENDING：PENDING态。当前entry正在ARP请求，但是还没收到ARP响应。
ETHARP_STATE_STABLE：有效态。当前entry记录的IP地址与MAC地址映射有效。
ETHARP_STATE_STABLE_REREQUESTING_1：有效过渡态1。就是为了防止entry块过期前频繁发起ARP请求。
ETHARP_STATE_STABLE_REREQUESTING_2：有效过渡态2。
ETHARP_STATE_STATIC：静态条目。手动配置的ARP映射，一直有效。
*/
#define ARP_FREE 1
#define ARP_WAITING 2
#define ARP_RESOLVED 3

#define ARP_REQ_RETRY 4

/*
arp缓存表： https://www.cnblogs.com/lizhuming/p/16845687.html
 - 每台主机or路由器内都有一个arp缓存表
 - 网络层的ip数据包需要经过数据链路层时，可以查询arp缓存表，如果表中没有，那么就会在链路层局域网内进行广播，查询
 这个IP对于的设备。
 - 如果ip层发送数据包时在arp缓存中找不到,那么就得先发arp请求. 此时就得先把数据包挂起来,等后面arp响应获取到mac后再发送.
*/
struct arpentry {
    struct list_head ae_list;          /* packet pending for hard address */
    struct netdev *ae_dev;             /* associated net interface */
    int ae_retry;                      /* arp reuqest retrying times */
    int ae_ttl;                        /* entry timeout */
    unsigned int ae_state;             /* entry state */
    unsigned short ae_pro;             /* L3 protocol supported by arp */
    unsigned int ae_ipaddr;            /* L3 protocol address(ip) */
    unsigned char ae_hwaddr[ETH_ALEN]; /* L2 protocol address(ethernet) */
};

/* arp format */
#define ARP_HRD_ETHER 1

#define ARP_OP_REQUEST 1   /* ARP request */
#define ARP_OP_REPLY 2     /* ARP reply */
#define ARP_OP_RREQUEST 3  /* RARP request */
#define ARP_OP_RREPLY 4    /* RARP reply */
#define ARP_OP_INREQUEST 8 /* InARP request */
#define ARP_OP_INREPLY 9   /* InARP reply */

#define ARP_HRD_SZ sizeof(struct arp)

/*
 * +-------------------+-------------------+-------------------+-------------------+
 * | Hardware Type (2) | Protocol Type (2) | Hardware Length (1)| Protocol Length (1)|
 * +-------------------+-------------------+-------------------+-------------------+
 * | Opcode (2)        | Sender MAC Address (6)                                      |
 * +-------------------+-------------------+-------------------+-------------------+
 * | Sender IP Address (4)                                                        |
 * +-------------------+-------------------+-------------------+-------------------+
 * | Target MAC Address (6)                                                      |
 * +-------------------+-------------------+-------------------+-------------------+
 * | Target IP Address (4)                                                        |
 * +-------------------+-------------------+-------------------+-------------------+
 */
struct arp {
    unsigned short arp_hrd;                  /* hardware address type */
    unsigned short arp_pro;                  /* protocol address type */
    unsigned char arp_hrdlen;                /* hardware address lenth */
    unsigned char arp_prolen;                /* protocol address lenth */
    unsigned short arp_op;                   /* ARP opcode(command) */
#if defined(ARP_ETHERNET) && defined(ARP_IP) /* only support ethernet & ip */
    unsigned char arp_sha[ETH_ALEN];         /* sender hw addr */
    unsigned int arp_sip;                    /* send ip addr */
    unsigned char arp_tha[ETH_ALEN];         /* target hw addr */
    unsigned int arp_tip;                    /* target ip addr */
#else
    unsigned char arp_data[0]; /* arp data field */
#endif
} __attribute__((packed));

static inline void arp_hton(struct arp *ahdr) {
    ahdr->arp_hrd = _htons(ahdr->arp_hrd);
    ahdr->arp_pro = _htons(ahdr->arp_pro);
    ahdr->arp_op = _htons(ahdr->arp_op);
}
#define arp_ntoh(ahdr) arp_hton(ahdr)

extern void arp_cache_traverse(void);
extern void arp_cache_init(void);
extern void arp_timer(int delta);
extern void arp_proc(int);

extern struct arpentry *arp_alloc(void);
extern struct arpentry *arp_lookup(unsigned short, unsigned int);
extern struct arpentry *arp_lookup_resolv(unsigned short, unsigned int);
extern int arp_insert(struct netdev *, unsigned short, unsigned int, unsigned char *);

extern void arp_queue_drop(struct arpentry *);
extern void arp_queue_send(struct arpentry *);
extern void arp_request(struct arpentry *);
extern void arp_in(struct netdev *dev, struct pkbuf *pkb);

#endif /* arp.h */
