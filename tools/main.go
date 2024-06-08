package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"

	"github.com/HLhuanglang/EasyNet/tools/cmd"
)

func main() {
	// 设置日志输出到文件
	executablePath, err := os.Executable()
	if err != nil {
		fmt.Println("Error getting executable path:", err)
		return
	}
	executableDir := filepath.Dir(executablePath)             // 获取可执行文件所在的目录路径
	logFilePath := filepath.Join(executableDir, "eTools.log") // 构建日志文件的完整路
	logFile, err := os.OpenFile(logFilePath, os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0666)
	if err != nil {
		log.Fatal("open eTool.log fail:", err)
	}
	defer logFile.Close()
	log.SetOutput(logFile)

	// 执行命令
	cmd.Execute()
}
