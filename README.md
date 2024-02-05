# EasyNet

## 1.序言

我认为网络编程的能力对于一名优秀的后端开发来说是必要的。后台开发，无非就是server串server，服务之间定义一些业务协议，然后通过tcp传输，这种模式能覆盖绝大部分的产品。EasyNet的目标主要是用于学习网络编程中的各自细节，理解tcp是流式协议这一个概念，学会如何处理七层协议的组包拆包等

## 2.支持平台

- linux

## 3.依赖

- gcc>=4.8.1(支持c++11)
- git
- cmake

## 4.编译运行

```
git clone --recurse-submodules https://github.com/HLhuanglang/EasyNet.git
cmake -S . -B build
cmake --build build
```

## 5.目录说明

| 目录         | 作用                                               |
| ------------ | -------------------------------------------------- |
| .vscode      | vscode开发环境配置                                 |
| config       | 项目配置文件                                       |
| docs         | 开发文档                                           |
| easy_net     | 网络库底层，目前主要支持tcp协议，udp暂未考虑       |
| raw_examples | socket系列接口的原生使用方法、优秀的网络库使用案例 |
| test         | 测试代码                                           |
| build        | libeasy_net.so/.a 编译的中间产物、可执行程序等     |
| third_party      | 依赖使用的第三方库                                 |

## 6.参考项目/文章/论文

[待补充]

## 7.授权许可
本项目采用 MIT 开源授权许可证，完整的授权说明已放置在 [LICENSE](LICENSE) 文件中。
