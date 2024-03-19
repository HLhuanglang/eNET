#!/bin/bash

# 目标 IP 地址和端口
TARGET_IP="127.0.0.1"
TARGET_PORT="8888"

# 创建一个名为 "send_message" 的函数，用于发送消息
send_message() {
    echo "hello,world $1" | nc $TARGET_IP $TARGET_PORT
}

# 使用 "for" 循环创建 100 个并发连接
for i in {1..100}; do
    send_message ${i} &
done

# 等待所有子进程完成
wait