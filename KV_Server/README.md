> File Name:     基于CS架构实现的KV跳表数据库
> Author:        Lihua
> Created Time:  2021.12.15   
> Finish Time:      2021.12.19	
> Reference:     https://github.com/youngyangyang04/Skiplist-CPP

# 基于CS架实现的KV跳表数据库

## 实现

1.使用跳表作为底层的存储数据结构。

2.使用了TCP协议

3.使用和 Redis 相同的语法，进行增删改查。

4.使用网络编程技术，将该程序部署到服务器。

- 基于C-S架构封装了Client , Server;

```c++
class Client
{
public:
    Client() { finished = true; };
    int sClient;          // fd
    sockaddr_in myClient; //地址信息
    myDB db;       //客户端对应的数据库
    bool finished; // 客户端是否还有用
};
```

```c++
class Server
{
public:
    int sListen;
    sockaddr_in local;
    //端口号
    int port;
    //服务端存储资源的数据库
    //数据库数组 每个元素是一个MyDB类型的指针
    myDB *DB[dbNum];
    //请求集合
    //用一个map表示存储在服务端的命令集合
    // key为命令的名字，value为一个指向函数的指针，根据请求的名字指向对应的函数
    std::map<string, RequestFun> requests;
};
```

- Request请求类，供C-S两端解析字符串

```c++
class Request
{
public:
    inline string get_request() const { return request; };
    Request(string s) : request(s){};
    bool is_valid_request();                  // 判断请求是否有效
    void split_request();                     // 解析请求
    void print_error(int error_number) const; //输出错误日志
    std::vector<string> arg;                  //请求数组

private:
    string request;
};
```

- Node 数据库跳表节点

```c++
// 节点类
template <typename K, typename V>
class Node
{
public:
    Node() {}
    Node(K k, V v, int);

    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V);

    // 用于保存指向 不同级别 下一个节点的 指针的线性数组
    Node<K, V> **forward;

    // 节点级别
    int node_level;

private:
    K key;
    V value;
};
```

- 数据库类

```c++
// 跳表类
template <typename K, typename V>
class SkipList
{
public:
    SkipList(int max_level = 8);
    ~SkipList();

    Node<K, V> *create_node(K, V, int); // 创建一个节点

    bool insert_element(const K key, const V value);  // 插入元素
    bool search_element(const K key, V &value) const; // 查找元素
    bool delete_element(const K key);                 // 删除元素
    bool delete_revoke();                             // 撤销上一次操作
    bool clear_history();                             // 清空历史操作

    void display_list(V &value) const; //展示跳表
    int size() const;                  //获取元素个数

    void load_file(); // 下载文件       从磁盘加载数据到内存中
    void dump_file(); // 上传文件       将内存中的数据转储到磁盘文件中

    std::string get_file_name() const;                            // 获取文件名
    void set_file_name(const std::string &str, const bool &flag); //设置文件名

private:
    // 从字符串解析出 K V
    void get_key_value_from_string(const std::string &str, std::string *key, std::string *value);
    // 检测字符串是否有效
    bool is_valid_string(const std::string &str);
    //获取随机级别
    int get_random_level() const;
    // 清空当前缓冲区跳表
    bool clear();
    // 插入元素,撤销时使用
    bool insert_element(Node<K, V> *insertNode);

private:
    // 跳表最大级别  初始化时赋值
    int max_level;

    // 头节点
    Node<K, V> *header;

    // 被删除链表的头节点
    std::stack<Node<K, V> *> delete_header;
    // 插入栈
    std::stack<Node<K, V> *> insert_header;
    //最后一次操作行为
    std::stack<ACTION> action;

    // 节点数目
    int element_count;

    // 跳表当前级别     元素存在的最大级别
    int skip_list_level;

    // 文件操作
    std::ifstream reader; // 读文件
    std::ofstream writer; // 写文件

    std::string store_file = "dumpFile";
};
```



## 效果

```shell
sh build.sh	#构建程序
```

指令解释

```c++
插入元素	//set key vulue
获取元素值	//get key
删除元素	//del key
下载	//load
上传，保存	//dump
显示	//display
撤销	//revoke
清空历史	//clear
获得大小	// size
获取文件名	//getName
更改文件名	//setName fileName 1/0
```

## 文件

### client文件夹

1. Client.cpp：main函数在该文件中,Client对象对连接进行管理，运行该程序后 Client会默认连接到服务器的 8888 端口。
2. Request.h ：定义了 一下服务器支持处理的 11种指令。在客户端向服务器发送请求前，会根据Request中的指令，检查请求是否合理。

```c++
"set", "get", "del", "load", "dump", "display", "revoke", "clear", "size", "getName", "setName"
```

### server文件夹

1. Skiplist.h：实现了了跳表结构,和原来Skiplist.h的不同，封装了一系列新的接口，晚上了数据逻辑，保证了线程安全
2. Server.h ：定义了Server类和 Client类 ，Client负责对socket和 pthread 进行管理。Server负责对Client和数据库进行管理.
3. Server.cpp：main函数在该文件中，Server中的socket负责监听可能的客户端连接，新的的连接会被绑定到client的socket上，并新建一个子进程来处理客户端的请求.
4. Request.h ：定义了 一下服务器支持处理的 11种指令。并给出了通过客户端发来的一串string 解析出指令的方法。

```c++
"set", "get", "del", "load", "dump", "display", "revoke", "clear", "size", "getName", "setName"
```

### bin 文件夹

存放可执行文件 KvServer KvClient 默认端口 8888

## 待优化

1. 可以基于 openssl 数字签名做安全检测
2. 基于raft一致性协议做分布式存储
3. 目前是K = string ,V =string 做存储，增删改查消耗更多cpu。
4. 可以将K = string 改为 int 性能和可靠性会更好。或 其他自定义类型，并重载operator <
5. Request Skiplist 提供virtual function,由继承类去实现功能扩展
6. 提供线程池和连接池维持线程分配和数据库连接