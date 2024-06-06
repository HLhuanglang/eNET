package cmd

import (
	"log"
	"strings"

	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
	"github.com/google/gopacket/pcap"
	"github.com/spf13/cobra"
)

var (
	device   string = "eth0" //网卡设备名
	filter   string = ""     //过滤器
	pkgcnt   int16  = 0      //捕获数据包的个数，0表示一直捕获
	snapshot int32  = 65535  //读取一个数据包的最大值，一般设置成这65535即可
	promisc  bool   = true   //是否开启混杂模式
)

var ethfilterCmd = &cobra.Command{
	Use:   "ethfilter",
	Short: "Capture packets on a network interface, Support BPF filter",
	Long: `Capture packets on a network interface, Support BPF filter.

PS:if your svr/cli running in the same machine, you can't capture the packets
beacuse the packets are not send to the network interface`,
	Example: `
eg1: eTools ethfilter -d eth0 tcp and port 80 //use bpf filter
eg2: eTools ethfilter -d eth0 //capture all packets
eg3: eTools ethfilter -d eth0 -c 10 //capture 10 packets
	`,
	Run: func(cmd *cobra.Command, args []string) {
		//获取一个网卡句柄
		handle, err := pcap.OpenLive(device, snapshot, promisc, pcap.BlockForever)
		if err != nil {
			log.Fatal(err)
		}
		defer handle.Close()

		// 设置过滤器,像tcpdump设置过滤器一样的
		filter = strings.Join(args, " ")
		err = handle.SetBPFFilter(filter) //设置过滤器
		if err != nil {
			log.Printf("Error setting BPF filter[%+v] for devInterface %s: %v", filter, device, err)
			return
		}

		//NewPacketSource新建一个数据包数据源
		packetSource := gopacket.NewPacketSource(handle, handle.LinkType())

		//捕捉一个数据包
		// packet, err := packetSource.NextPacket() //返回一个数据包
		// if err != nil {
		// 	log.Fatal(err)
		// }
		// fmt.Println(packet)

		//捕捉数据包
		always := false
		if pkgcnt == 0 {
			always = true
		}
		captureCnt := 0
		p := packetSource.Packets() //返回一个channel
		for data := range p {
			if !always && captureCnt >= int(pkgcnt) {
				break
			}
			captureCnt++
			ipLayer := data.Layer(layers.LayerTypeIPv4)
			if ipLayer != nil {
				ip, _ := ipLayer.(*layers.IPv4)
				log.Printf("IP layer %+v\n", ip)
			}
			tcpLayer := data.Layer(layers.LayerTypeTCP)
			if tcpLayer != nil {
				tcp, _ := tcpLayer.(*layers.TCP)
				log.Printf("TCP layer %+v\n", tcp)
			}
			appLayer := data.ApplicationLayer()
			if appLayer != nil {
				log.Printf("Application layer %+v\n\n", appLayer.Payload())
			}
		}
	},
}

func init() {
	rootCmd.AddCommand(ethfilterCmd)
	ethfilterCmd.Flags().StringVarP(&device, "device", "d", "eth0", "device name")
	ethfilterCmd.Flags().Int16VarP(&pkgcnt, "count", "c", 1, "capture packet count, 0 means always capture")
}
