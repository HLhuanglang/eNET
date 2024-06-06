package cmd

import (
	"fmt"
	"log"
	"net"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"

	"github.com/spf13/cobra"
)

const (
	message = "Ping"
)

var (
	ip   string
	port string
	gcnt int
)

var echoCmd = &cobra.Command{
	Use:   "echo",
	Short: "Stress test echo server",
	Example: `
	eg1: eTools echo -i 127.0.0.1 -p 8888 -g 1000`,
	Run: func(cmd *cobra.Command, args []string) {
		wg := sync.WaitGroup{}
		var count int64
		for i := 1; i <= gcnt; i++ {
			wg.Add(1)
			go func(i int) {
				defer wg.Done()
				if !SocketClient(ip, port, i) {
					atomic.AddInt64(&count, 1)
				}
			}(i)
		}
		wg.Wait()
		fmt.Printf("Total:%d Fail:%d\n", gcnt, count)
	},
}

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
		log.Printf("Write Err:%+v\n", err)
		return false
	}

	// 读取返回消息
	buff := make([]byte, 1024)
	n, err := conn.Read(buff)
	if err != nil {
		log.Printf("Read Err:%+v\n", err)
		return false
	}
	recv_msg := string(buff[:n])
	if recv_msg != msg {
		log.Printf("LocalAddr=%+v send_msg=%v recv_msg=%v\n", conn.LocalAddr().String(), msg, recv_msg)
		return false
	}
	return true
}

func init() {
	rootCmd.AddCommand(echoCmd)
	echoCmd.Flags().StringVarP(&ip, "ip", "i", "", "server ip")
	echoCmd.Flags().StringVarP(&port, "port", "p", "", "server port")
	echoCmd.Flags().IntVarP(&gcnt, "goroutine", "g", 1, "req goroutine")
}
