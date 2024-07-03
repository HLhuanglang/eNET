<p align="center"><img src="https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/20240418114045.png"></p>

# EasyNet

## 1.序言

我认为网络编程的能力对于一名优秀的后端开发来说是必要的。后台开发，无非就是server串server，服务之间定义一些业务协议，然后通过tcp传输，这种模式能覆盖绝大部分的产品。EasyNet的目标主要是用于学习网络编程中的各自细节，理解tcp是流式协议这一个概念，学会如何处理七层协议的组包拆包等

## 2.支持平台

- linux
- windows(doing)

## 3.依赖

- gcc>=4.8.1(支持c++11)
- git
- cmake
- geogletest(可选)
- spdlog(必须)：如果使用gcc4.8.1编译失败,见[issues-3050](https://github.com/gabime/spdlog/issues/3050)

## 4.编译运行

```
git clone https://github.com/HLhuanglang/EasyNet.git
cmake -S . -B build
cmake --build build
```

编译选项

| 选项                   | 描述                       | 默认值 |
| ---------------------- | -------------------------- | ------ |
| WITH_DNS         | 是否支持dns协议            | OFF    |
| WITH_HTTP        | 是否支持http协议           | OFF    |
| BUILD_EXAMPLES    | 是否编译使用案例           | OFF    |
| BUILD_RAWEXAMPLES | 是否编译原始socket使用案例 | OFF    |
| BUILD_UNITTEST    | 是否编译单元测试           | OFF    |



## 5.目录说明

| 目录         | 作用                                               |
| ------------ | -------------------------------------------------- |
| cmake        | cmake安装模板文件|
| docs         | 开发文档                                           |
| easy_net     | 网络库底层，目前主要支持tcp协议，udp暂未考虑       |
| examples     | 使用easynet编写的demo       |
| raw_examples | socket系列接口的原生使用方法、优秀的网络库使用案例 |
| test         | 测试代码                                           |
| tools      | 工具                                 |

## 6.参考项目/文章/论文

- [Easy-Reactor](https://github.com/LeechanX/Easy-Reactor)
- [muduo](https://github.com/chenshuo/muduo)
- [sim_muduo](https://gitee.com/coolbaul/sim_muduo)
- [trantor](https://github.com/an-tao/trantor)

## 7.授权许可
本项目采用 MIT 开源授权许可证，完整的授权说明已放置在 [LICENSE](LICENSE) 文件中。
