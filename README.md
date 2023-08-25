# EasyNet

## 1.序言

我认为网络编程的能力对于一名优秀的后端开发来说是必要的。后台开发，无非就是server串server，服务之间定义一些业务协议，然后通过tcp传输，这种模式能覆盖绝大部分的产品。EasyNet的目标主要是用于学习网络编程中的各自细节，理解tcp是流式协议这一个概念，学会如何处理七层协议的组包拆包等

## 2.支持平台

- linux

## 3.依赖

- gcc>=4.8.1(支持c++11)
- git(拉取代码)
- bear(用于生成compile_commands.json)

## 4.编译运行

### 4.1获取代码

```
git clone https://github.com/HLhuanglang/EasyNet.git
```

### 4.2配置模块

```
cd /path/to/you/EasyNet
./configure

//默认输出如下
root@Holo-wo:/path/to/you/EasyNet$ ./configure 
[easy_net_compile_config.mk]

PREFIX=/opt/easy_net
WITH_HTTP=yes
WITH_MQTT=yes
ENABLE_UDS=no
CONFIG_DATE=20230213
```

使用`configure --help`可以查看支持的选项

### 4.3生成头文件依赖
```
./scripts/makefilevars.sh >> makefilevars.mk
```

### 4.4编译选项

- `make easy_net`：只编译libeasy_net.a/so
- `make all`：编译easy_net库、测试程序、一些原始socket接口使用案例
- `make raw_examples`：只编译原始socket使用案例
- `make test`：测试用例，会自动编译easy_net

## 5.开发环境

开发环境因人而已，可自由配置，本人使用的是vscode+remote的形式，所以项目中提供了一些配置用于设置vscode，相关配置也做简要阐述。

### 5.1智能提示+代码格式化

未使用官方的c/cpp插件，选择了clangd。clangd的代码提示需要compile_commands.json文件，所以初次编译项目时，可以选择**`bear make all`**进行生成。

### 5.2服务调试

- 修改.vscode目录下launch.json中program即可
- 由于程序使用多进程,直接调试会在各个线程中切换,所以可以在~/.gdbinit中配置
  - set scheduler-locking on

## 6.目录说明

| 目录         | 作用                                               |
| ------------ | -------------------------------------------------- |
| .vscode      | vscode开发环境配置                                 |
| config       | 项目配置文件                                       |
| docs         | 开发文档                                           |
| easy_net     | 网络库底层，目前主要支持tcp协议，udp暂未考虑       |
| protocol     | 用户层协议，例如http、websocket等                  |
| raw_examples | socket系列接口的原生使用方法、优秀的网络库使用案例 |
| test         | 测试代码                                           |
| build        | libeasy_net.so/.a 编译的中间产物、可执行程序等     |
| scripts      | 构建系统依赖的脚本                                 |

## 7.参考项目/文章/论文

[待补充]

## 8.授权许可
本项目采用 MIT 开源授权许可证，完整的授权说明已放置在 [LICENSE](LICENSE) 文件中。
