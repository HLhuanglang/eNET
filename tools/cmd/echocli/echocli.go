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
	"sync/atomic"
)

const (
	message = "Ping"
)

func SocketClient(ip, port string, index int) bool {
	// 根据端口拼接网络地址
	addr := strings.Join([]string{ip, port}, ":")

	// 连接服务器
	conn, err := net.Dial("tcp", addr)
	if err != nil {
		return false
	}
	defer func() {
		fmt.Println(conn.LocalAddr().String() + " Close")
		conn.Close()
	}()

	// 写入发送消息
	msg := message + "_" + strconv.Itoa(index)
	_, err = conn.Write([]byte(msg))
	if err != nil {
		fmt.Printf("Write Err:%+v\n", err)
		return false
	}

	// 读取返回消息
	buff := make([]byte, 1024)
	n, err := conn.Read(buff)
	if err != nil {
		fmt.Printf("Read Err:%+v\n", err)
		return false
	}
	recv_msg := string(buff[:n])
	if recv_msg != msg {
		fmt.Printf("LocalAddr=%+v send_msg=%v recv_msg=%v\n", conn.LocalAddr().String(), msg, recv_msg)
		return false
	}
	return true
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
	var count int64
	for i := 1; i <= cnt; i++ {
		//time.Sleep(1 * time.Millisecond)
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			if !SocketClient(ip, port, i) {
				atomic.AddInt64(&count, 1)
			}
		}(i)
	}
	wg.Wait()
	fmt.Printf("Total=%v Fail=%v\n", cnt, count)
}
