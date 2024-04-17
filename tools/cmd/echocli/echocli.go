package main

import (
	"flag"
	"log"
	"net"
	"os"
	"strconv"
	"strings"
	"sync"
)

const (
	message = "Ping"
)

func SocketClient(ip, port string, index int) {
	// 根据端口拼接网络地址
	addr := strings.Join([]string{ip, port}, ":")
	// 根据地址拨号
	conn, err := net.Dial("tcp", addr)

	if err != nil {
		log.Fatalln(err)
		os.Exit(1)
	}

	defer conn.Close()
	// 写入发送消息
	conn.Write([]byte(message + "_" + strconv.Itoa(index)))

	buff := make([]byte, 1024)
	// 循环读取消息，响应服务器
	_, _ = conn.Read(buff)
}

func main() {
	var ip string
	var port string
	var cnt int
	flag.IntVar(&cnt, "c", 5000, "req count")
	flag.StringVar(&ip, "h", "127.0.0.1", "server ip")
	flag.StringVar(&port, "p", "8888", "server port")
	flag.Parse()

	if !flag.Parsed() {
		log.Fatalln("flag parse error")
		os.Exit(1)
	}

	wg := sync.WaitGroup{}
	for i := 1; i <= cnt; i++ {
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			SocketClient(ip, port, i)
		}(i)
	}
	wg.Wait()
}
