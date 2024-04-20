package main

import (
	"flag"
	"fmt"
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

func SocketClient(ip, port string, index int) bool {
	// 根据端口拼接网络地址
	addr := strings.Join([]string{ip, port}, ":")

	conn, err := net.Dial("tcp", addr)

	if err != nil {
		fmt.Printf("err:%+v\n", err)
		return false
	}

	defer conn.Close()
	// 写入发送消息
	msg := message + "_" + strconv.Itoa(index)
	_, err = conn.Write([]byte(msg))
	if err != nil {
		fmt.Printf("Write Err:%+v\n", err)
		return false
	}

	buff := make([]byte, 1024)
	// 循环读取消息，响应服务器
	n, err := conn.Read(buff)
	if err != nil {
		fmt.Printf("Read Err:%+v\n", err)
		return false
	}
	recv_msg := string(buff[:n])
	fmt.Printf("LocalAddr=%+v send_msg=%v recv_msg=%v\n", conn.LocalAddr().String(), msg, recv_msg)
	return recv_msg == msg
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
		//time.Sleep(1 * time.Millisecond)
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			if !SocketClient(ip, port, i) {
				return
			}
		}(i)
	}
	wg.Wait()
}
