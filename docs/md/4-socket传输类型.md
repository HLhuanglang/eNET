# socket传输类型

## 文本类型

### http


## 二进制类型

### c/c++自定义结构体,类对象

使用结构或类对象需要收发双方都有定义,原理在于结构体或者对象在内存中都是一块连续的区域。

```cpp
struct Student
{
	int iId;
	string strName;
	bool bSex;    //为了节省内存空间，性别采用一个字节的BOOL类型表示
};
```

客户端
```cpp
struct Student stu;    //声明一个Student结构体变量
stu.iId = 1001;
stu.bSex = true;       //true表示男性，false表示女性，你反过来也行，别打拳
stu.strName = "abcdefzzzzz";
 
//下面的m_sclient是客户端（发送方）的Socket套接字
 
//方法一：推荐如下
send(m_sclient, (char*)&stu, sizeof(Student), 0);//&stu取stu地址，(char*)转化为char类型的指针
 
//方法二：或者增加一个中间变量sendBuff[]来传送，如下
//char sendBuff[1024];
//memset(sendBuff,0,sizeof(sendBuff));
//memcpy(sendBuff, &stu, sizeof(Student));
//send(m_sclient, sendBuff, sizeof(sendBuff), 0);
```

服务端
```cpp
struct Student stu;    //声明一个结构体变量，用于接收客户端（发送方）发来的数据
char buffFromClient[1024];    //用于临时接收发送方的数据
//方法一：（推荐）
recv(clientSocket, (char*)&stu, sizeof(Student), 0);
 
//方法二：
//memset(buffFromClient,0,sizeof(buffFromClient));
//recv(clientSocket, buffFromClient, sizeof(Student), 0);
//memset(&stu,buffFromClent,sizeof(Student));
```

### pb
pb提供了序列化的结构

```cpp
class MessageLite {
public:
	//序列化：
	bool SerializeToOstream(ostream* output) const;
	bool SerializeToArray(void *data, int size) const;    //使用这个接口就能发送了
	bool SerializeToString(string* output) const;
	
	//反序列化：
	bool ParseFromIstream(istream* input);
	bool ParseFromArray(const void* data, int size);
	bool ParseFromString(const string& data);
};
```
