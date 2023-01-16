# EasyNet

## 支持平台
- linux

## 运行要求
- 编译器支持c++11环境

ide使用vscode，配合clangd+error lens能够非常愉快的编写代码
- clangd：用于代码跳转、智能补充、提示错误
- bear：生成compile_commands.json

## 编译
- `bear make all`：编译动态库、测试程序、一些原始socket接口使用案例
- `make raw_examples`：原始socket使用案例
- `make easy_net`：只编译动态库
- `make example`：如何使用easy_net进行tcp、udp服务的开发案例
- `make test`：测试用例

## 调试
- 修改.vscode目录下launch.json中program即可
- 由于程序使用多进程,直接调试会在各个线程中切换,所以可以在~/.gdbinit中配置
    - set scheduler-locking on

## 目录说明

| 目录         | 作用                                               |
| ------------ | -------------------------------------------------- |
| .vscode      | vscode一些调试配置                                 |
| config       | 配置文件模板(暂未使用)                             |
| docs         | 开发文档                                           |
| easy_net     | 网络库底层，目前主要支持tcp协议，udp暂未考虑       |
| protocol     | 用户层协议，例如http、websocket等                  |
| raw_examples | socket系列接口的原生使用方法、优秀的网络库使用案例 |
| test         | 测试代码|
| output       | libeasy_net.a和libeasy_net.so以及公开的头文件      |
| build        | 编译的中间产物、可执行程序等                       |