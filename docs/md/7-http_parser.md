# http_parser使用

解析tcp收的包：

- Q1：头部+不完整body
- Q2：头部+完整body
- Q3：头部+完整body+其他多余数据

## Q1

- 如果parser没有被重置
    - 第二次解析的时候重新传入整个完整消息，还是只按照conten-length的长度来读取body。
    - 第二次解析传入剩余消息，则只有on_body回调被调用

- 如果parser被重置了
    - 第二次解析重传整个完整消息，则会重新解析 
    - 第二次传入body剩余的部分，直接不解析
