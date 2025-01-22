#include "ether.h"
#include "icmp.h"
#include "ip.h"
#include "lib.h"
#include "netif.h"
#include "raw.h"
#include "route.h"
#include "tcp.h"
#include "udp.h"

void ip_recv_local(struct pkbuf *pkb) {
    struct ip *iphdr = pkb2ip(pkb);

    /* fragment reassambly */
    if (iphdr->ip_fragoff & (IP_FRAG_OFF | IP_FRAG_MF)) {
        if (iphdr->ip_fragoff & IP_FRAG_DF) {
            ipdbg("error fragment");
            free_pkb(pkb);
            return;
        }
        pkb = ip_reass(pkb);
        if (!pkb)
            return;
        iphdr = pkb2ip(pkb);
    }

    /* copy pkb to raw */
    raw_in(pkb);

    /* pass to upper-level */
    /*
     常见值：
        0: 保留
        1: ICMP, Internet Control Message [RFC792]
        2: IGMP, Internet Group Management [RFC1112]
        3: GGP, Gateway-to-Gateway [RFC823]
        4: IP in IP (encapsulation) [RFC2003]
        6: TCP Transmission Control Protocol [RFC793]
        17: UDP User Datagram Protocol [RFC768]
        20: HMP Host Monitoring Protocol [RFC 869]
        27: RDP Reliable Data Protocol [ RFC908 ]
        46: RSVP (Reservation Protocol)
        47: GRE (General Routing Encapsulation)
        50: ESP Encap Security Payload [RFC2406]
        51: AH (Authentication Header) [RFC2402]
        54: NARP (NBMA Address Resolution Protocol) [RFC1735]
        58: IPv6-ICMP (ICMP for IPv6) [RFC1883]
        59: IPv6-NoNxt (No Next Header for IPv6) [RFC1883]
        60: IPv6-Opts (Destination Options for IPv6) [RFC1883]
        89: OSPF (OSPF Version 2) [RFC 1583]
        112: VRRP (Virtual Router Redundancy Protocol) [RFC3768]
        115: L2TP (Layer Two Tunneling Protocol)
        124: ISIS over IPv4
        126: CRTP (Combat Radio Transport Protocol)
        127: CRUDP (Combat Radio User Protocol)
        132: SCTP (Stream Control Transmission Protocol)
        136: UDPLite [RFC 3828]
        137: MPLS-in-IP [RFC 4023]
    */
    switch (iphdr->ip_pro) {
        case IP_P_ICMP:  // icmp协议是在ip协议之上的，ip首部+payload，其中payload就是icmp报文
            icmp_in(pkb);
            break;
        case IP_P_TCP:
            tcp_in(pkb);
            break;
        case IP_P_UDP:
            udp_in(pkb);
            break;
        default:
            free_pkb(pkb);
            ipdbg("unknown protocol");
            break;
    }
}

void ip_recv_route(struct pkbuf *pkb) {
    // 查询路由缓存表,有最好,没有就回icmp包不可达告诉上层
    if (rt_input(pkb) < 0)
        return;

    /* Is this packet sent to us? */
    if (pkb->pk_rtdst->rt_flags & RT_LOCALHOST) {
        ip_recv_local(pkb);  // 确实是发送给本机的包,那就进行解析.
    } else {
        ip_hton(pkb2ip(pkb));
        ip_forward(pkb);  // 转发,发送给网关(路由器),交由路由器进行路由选择,将ip包发送出去.
    }
}

void ip_in(struct netdev *dev, struct pkbuf *pkb) {
    struct ether *ehdr = (struct ether *)pkb->pk_data;
    struct ip *iphdr = (struct ip *)ehdr->eth_data;
    int hlen;

    /* Fussy sanity check */
    if (pkb->pk_type == PKT_OTHERHOST) {
        ipdbg("ip(l2) packet is not for us");
        goto err_free_pkb;
    }

    if (pkb->pk_len < ETH_HRD_SZ + IP_HRD_SZ) {
        ipdbg("ip packet is too small");
        goto err_free_pkb;
    }

    if (ipver(iphdr) != IP_VERSION_4) {
        ipdbg("ip packet is not version 4");
        goto err_free_pkb;
    }

    hlen = iphlen(iphdr);
    if (hlen < IP_HRD_SZ) {
        ipdbg("ip header is too small");
        goto err_free_pkb;
    }

    if (ip_chksum((unsigned short *)iphdr, hlen) != 0) {
        ipdbg("ip checksum is error");
        goto err_free_pkb;
    }

    ip_ntoh(iphdr);
    if (iphdr->ip_len < hlen ||
        pkb->pk_len < ETH_HRD_SZ + iphdr->ip_len) {
        ipdbg("ip size is unknown");
        goto err_free_pkb;
    }

    if (pkb->pk_len > ETH_HRD_SZ + iphdr->ip_len)
        pkb_trim(pkb, ETH_HRD_SZ + iphdr->ip_len);

    /* Now, we can take care of the main ip processing safely. */
    ipdbg(IPFMT " -> " IPFMT "(%d/%d bytes)",
          ipfmt(iphdr->ip_src), ipfmt(iphdr->ip_dst),
          hlen, iphdr->ip_len);
    ip_recv_route(pkb);
    return;

err_free_pkb:
    free_pkb(pkb);
}
