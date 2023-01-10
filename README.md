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