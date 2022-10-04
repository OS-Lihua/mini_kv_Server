/* ************************************************************************
> File Name:     client.cpp
> Author:        Lihua
> Created Time:  2021.12.05
> Finish Time:   2021.12.07
> Reference:     https://github.com/youngyangyang04/Skiplist-CPP
 ************************************************************************/
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "request.h"

using std::cout;
using std::endl;
using std::string;

//定义程序中使用的常量
#define SERVER_ADDRESS "" //服务器端IP地址
#define MSGSIZE 2048      //收发缓冲区的大小
#define PORT 8888         //默认端口号

int main(int argc, char **argv)
{
    short port = PORT;

    if (argc >= 2)
    {
        port = atoi(argv[1]);
    }

    //创建套接字文件描述符
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //创建套接字结构体
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    int ret = connect(socket_fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect error");
        exit(1);
    }
    else
    {
        cout << "Redis_client by Stack start-up..." << endl;
    }
    while (1)
    {
        cout << "\nStack_Redis>\n>";

        //从键盘输入
        string writebuffer;
        getline(std::cin, writebuffer);
        std::cout << writebuffer << std::endl;

        Request r(writebuffer);

        if (!r.is_valid_request())
        {
            continue;
        }
        // 发送数据
        write(socket_fd, writebuffer.c_str(), writebuffer.length());
        writebuffer.clear(); //清空
        char readbuffer[MSGSIZE] = {0};
        int ret = read(socket_fd, readbuffer, MSGSIZE);

        if (ret <= 0)
        {
            printf("服务器端关闭了连接\n");
            break;
        }
        if (ret == -1)
        {
            perror("read error");
            exit(1);
        }
        //输出收到的数据
        cout << "Server reply: \n"
             << readbuffer << endl;
        if (std::string(readbuffer).compare(std::string("exit"))  == 0 )
        {
            std::cout << "exit succeed" << std::endl;
            break;
        }
        memset(readbuffer, 0, MSGSIZE);
    }
    close(socket_fd);
    return 0;
}
