# tun/tap设备

虚拟网卡<--tun/tap驱动:包括网卡驱动和字符设备驱动

## 什么是tun/tap
tun和tap是操作系统内核中的虚拟网络设备(虚拟网卡)，用软件模拟实现，提供和硬件设备完全相同的功能
- tap：等同于以太网设备，操作第二层数据包，例如以太网帧
- tun：模拟网络层设备，操作第三层数据包，如ip数据报

## 如何使用tun/tap

使用tun/tap设备的示例程序(摘自openvpn开源项目 http://openvpn.sourceforge.net ，tun.c 文件)
```c
int open_tun (const char *dev, char *actual, int size)
{
    struct ifreq ifr; int fd;
    char *device = "/dev/net/tun";
    if ((fd = open(device, O_RDWR)) < 0)   // 创建描述符
        msg(M_ERR, "Cannot open TUN/TAP dev %s", device);

    memset(&ifr, 0, sizeof (ifr));
    ifr.ifr_flags = IFF_NO_PI;
    if (!strncmp(dev, "tun", 3)) {
        ifr.ifr_flags |= IFF_TUN;
    } else if (!strncmp(dev, "tap", 3)) {
        ifr.ifr_flags |= IFF_TAP;
    } else {
        msg(M_FATAL, "I don't recognize device %s as a TUN or TAP device",dev);
    }
    if (strlen(dev) > 3)    /* unit number specified? */
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    if (ioctl(fd, TUNSETIFF, (void *)&ifr) < 0)     //打开虚拟网卡
        msg(M_ERR, "Cannot ioctl TUNSETIFF %s", dev);

    set_nonblock(fd);
    msg(M_INFO, "TUN/TAP device %s opened", ifr.ifr_name);
    strncpynt(actual, ifr.ifr_name, size);

    return fd;
}
```
通过ifconfig就能查看到虚拟网卡信息了，后续操作使用read/write对fd进行读写

## 原理

![](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/20250121172710.png)
