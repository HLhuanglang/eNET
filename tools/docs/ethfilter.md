# 网卡抓包工具

网卡驱动将数据链路层的包后copy了一份，通过原始套接字丢给了libpacp库，然后gopacket库做了一些ip组包之类的操作，提供了一个方便使用的接口。整个抓包结构大概如下：

![](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/1717839294964(1).jpg)

## 1. 安装pcap库
- On Ubuntu or other Debian-based systems, you can use the following command:

  ```bash
  sudo apt-get install libpcap-dev
  ```

- On CentOS, Fedora, or other RHEL-based systems, you can use the following command:

  ```bash
  sudo yum install libpcap-devel
  ```

## 2. 使用gopacket