/* ************************************************************************
> File Name:     server.h
> Author:        Lihua
> Created Time:  2021.12.05
> Finish Time:   2021.12.07
> Reference:     https://github.com/youngyangyang04/Skiplist-CPP
 ************************************************************************/

#ifndef __SERVER_H__
#define __SERVER_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>

#include "skiplist.h"
#include "request.h"

using std::cout;
using std::endl;
using std::string;
class Client;
class Server;

//数据库的个数
const int dbNum = 16;

//定义在redis中需要使用到的数据结构
//数据库使用一个dict表示
typedef SkipList<string, string> myDB;

//对请求的函数指针进行typedef
//用请求的函数指针表示一个命令集合
using RequestFun = void *(*)(Server *server, Client *client, string key, string &value, bool &flag);

class Client
{
public:
    Client() { finished = true; };
    int sClient;          // fd
    sockaddr_in myClient; //地址信息
    myDB db;       //客户端对应的数据库
    pthread_t tid; //线程号
    bool finished; // 客户端是否还有用
};

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

#endif // ! __SERVER_H__