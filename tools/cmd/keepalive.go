package cmd

import (
	"io"
	"net/http"

	"github.com/spf13/cobra"
)

var addr string
var maxConn int

var keepaliveCmd = &cobra.Command{
	Use:   "keepalive",
	Short: "Http keepalive test",
	Run: func(cmd *cobra.Command, args []string) {
		for i := 0; i < 3; i++ {
			resp, err := http.Get("http://127.0.0.1:8888/Hi")
			if err != nil {
				panic(err)
			}
			// 空读，注：golang里一定要把数据读完，否则不会复用TCP连接
			_, _ = io.Copy(io.Discard, resp.Body)
			resp.Body.Close()
		}
	},
}

func init() {
	rootCmd.AddCommand(keepaliveCmd)
	keepaliveCmd.Flags().StringVarP(&addr, "addr", "a", "http://127.0.0.1:8888/Hi", "http url")
	keepaliveCmd.Flags().IntVarP(&maxConn, "maxConn", "m", 1, "max connection")
}
