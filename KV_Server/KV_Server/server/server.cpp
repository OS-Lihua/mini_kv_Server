/* ************************************************************************
> File Name:     server.cpp
> Author:        Lihua
> Created Time:  2021.12.05
> Finish Time:   2021.12.07
> Reference:     https://github.com/youngyangyang04/Skiplist-CPP
 ************************************************************************/

#include "server.h"

//默认端口号
#define PORT 8888
//缓冲区长度
const int MSGSIZE = 2048;
//最大连接数
const int MAXCLIENTNUM = 16;
typedef std::map<string, RequestFun> mymap;
typedef SkipList<string, string> myDB;

Server *server = new Server();

// set命令
void *setRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    flag = client->db.insert_element(key, value); //插入元素
    return nullptr;
}

// get命令
void *getRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    flag = client->db.search_element(key, value); //查找元素
    return nullptr;
}

// del命令
void *delRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    flag = client->db.delete_element(key); //删除元素
    return nullptr;
}

// load命令
void *loadRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    client->db.load_file(); //下载文件
    return nullptr;
}
// dump命令
void *dumpRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    client->db.dump_file(); //上传文件
    return nullptr;
}

// display命令
void *displayRequest(Server *server, Client *client, string key, string &str, bool &flag)
{
    client->db.display_list(str); //展示跳表
    return nullptr;
}

// revoke命令
void *revokeRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    flag = client->db.delete_revoke(); //撤销上一次操作
    return nullptr;
}

// clear命令
void *clearRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    flag = client->db.clear_history(); // 清空历史操作
    return nullptr;
}

// size命令
void *sizeRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    int *dbSize = new int(client->db.size());
    return (void *)dbSize; //获取元素个数
}

// getName命令
void *getNameRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    value = client->db.get_file_name(); //获取文件名
    return nullptr;
}

// setName命令
void *setNameRequest(Server *server, Client *client, string key, string &value, bool &flag)
{
    client->db.set_file_name(value, flag); //设置文件名
    return nullptr;
}

/////////////

//初始化数据库
void initDB()
{
    cout << "Redis by Stack start-up..." << endl;
}

//初始化命令库
using std::pair;
void initRequest(Server *&server)
{
    cout << "------ init Request start. ------" << endl;
    (server->requests).insert(pair<string, RequestFun>("set", &setRequest));
    (server->requests).insert(pair<string, RequestFun>("get", &getRequest));
    (server->requests).insert(pair<string, RequestFun>("del", &delRequest));
    (server->requests).insert(pair<string, RequestFun>("load", &loadRequest));
    (server->requests).insert(pair<string, RequestFun>("dump", &dumpRequest));
    (server->requests).insert(pair<string, RequestFun>("display", &displayRequest));
    (server->requests).insert(pair<string, RequestFun>("revoke", &revokeRequest));
    (server->requests).insert(pair<string, RequestFun>("clear", &clearRequest));
    (server->requests).insert(pair<string, RequestFun>("size", &sizeRequest));
    (server->requests).insert(pair<string, RequestFun>("getfile", &getNameRequest));
    (server->requests).insert(pair<string, RequestFun>("sitefile", &setNameRequest));
    cout << "------ init Request end. ------" << endl;
}

//初始化服务端
void initServer(Server *&server, int port)
{
    cout << "------ init Server start. ------" << endl;
    //创建客户端的套接字
    server->sListen = socket(AF_INET, SOCK_STREAM, 0);

    //初始化数据库
    initDB();

    //初始化命令库
    initRequest(server);

    //绑定
    (server->local).sin_family = AF_INET;
    (server->local).sin_port = htons(port);
    (server->local).sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(server->sListen, (struct sockaddr *)&(server->local), sizeof(server->local));
    if (ret == -1)
    {
        perror("listen fild :");
        exit(-1);
    }

    //监听
    ret = listen(server->sListen, 128);
    if (ret == -1)
    {
        perror("listen fild :");
        exit(-1);
    }

    cout << "------ init Server end. ------" << endl;
}

//工作线程
void *worker(void *arg)
{

    Client *c = (Client *)arg;

    int exitcount = 0;
    char readbuffer[MSGSIZE] = {0};
    int ret;
    while (true)
    {
        // ret是发送消息的字节长度
        // readbuffer是发送消息的内容

        ret = read(c->sClient, readbuffer, MSGSIZE);
        if (ret <= 0)
            return 0;

        readbuffer[ret] = '\0';

        cout << "client:" << inet_ntoa((c->myClient).sin_addr) << " : "
             << ntohs((c->myClient).sin_port) << "[ " << ret << " bytes ] :" << readbuffer << endl;

        //类型转换，方便比较
        string message(readbuffer);
        memset(readbuffer, 0, MSGSIZE);
        //对用户发送的消息进行判断
        //对接受的消息进行分割，查看是否符合get/set命令的格式
        //分隔符
        Request r(message);
        r.split_request();

        if (r.arg[0] == "set")
        {
            string key = r.arg[1];
            string value = r.arg[2];
            bool flag = true;
            mymap::iterator it;
            it = server->requests.find("set");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            //向客户端发送数据
            string sendMessage = "OK";
            char *str = new char[strlen(sendMessage.c_str())];
            strcpy(str, sendMessage.c_str());

            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            exitcount = 0;
            continue;
        }
        if (r.arg[0] == "get")
        {
            //符合命令格式
            //获得键值
            string key = r.arg[1];
            string value = "";
            bool flag = true;

            mymap::iterator it;
            it = server->requests.find("get");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            if (flag)
            {
                char *str = new char[strlen(value.c_str())];
                strcpy(str, value.c_str());
                write(c->sClient, str, strlen(str) + sizeof(char));
                delete[] str;
            }
            else
            {
                string sendMessage = "get null";
                char *str = new char[strlen(sendMessage.c_str())];
                strcpy(str, sendMessage.c_str());
                write(c->sClient, str, strlen(str) + sizeof(char));
                delete[] str;
            }
            continue;
        }
        if (r.arg[0] == "del")
        {

            string key = r.arg[1];
            string value = "";
            bool flag = true;
            mymap::iterator it;
            it = server->requests.find("del");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            string sendMessage;
            if (flag)
            {
                sendMessage = "delete succeed";
            }
            else
            {
                sendMessage = "delete faild";
            }
            char *str = new char[strlen(sendMessage.c_str())];
            strcpy(str, sendMessage.c_str());
            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            exitcount = 0;
            continue;
        }
        if (r.arg[0] == "load")
        {
            //符合命令格式
            //获得键值
            string key = "";
            string value = "";
            bool flag = true;
            mymap::iterator it;
            it = server->requests.find("load");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            // 向客户端发送数据
            string sendMessage = "loading succeed";
            char *str = new char[strlen(sendMessage.c_str())];
            strcpy(str, sendMessage.c_str());
            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            continue;
        }
        if (r.arg[0] == "dump")
        {
            //符合命令格式
            //获得键值
            string key = "";
            string value = "";
            bool flag = true;
            mymap::iterator it;
            it = server->requests.find("dump");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            // 向客户端发送数据
            string sendMessage = "dump succeed";
            char *str = new char[strlen(sendMessage.c_str())];
            strcpy(str, sendMessage.c_str());
            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            continue;
        }
        if (r.arg[0] == "display")
        {
            //符合命令格式
            //获得键值
            string key = "";
            string value = "";
            bool flag = true;
            mymap::iterator it;
            it = server->requests.find("display");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            // 向客户端发送数据
            char *str = new char[strlen(value.c_str())];
            strcpy(str, value.c_str());
            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            continue;
        }
        if (r.arg[0] == "revoke")
        {
            //符合命令格式
            //获得键值
            string key = "";
            string value = "";
            bool flag = true;
            mymap::iterator it;
            it = server->requests.find("revoke");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            // 向客户端发送数据
            string sendMessage = "revoke succeed";
            char *str = new char[strlen(sendMessage.c_str())];
            strcpy(str, sendMessage.c_str());

            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            exitcount = 0;
            continue;
        }
        if (r.arg[0] == "clear")
        {
            //符合命令格式
            //获得键值
            string key = "";
            string value = "";
            bool flag = true;
            mymap::iterator it;
            it = server->requests.find("clear");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            // 向客户端发送数据
            string sendMessage = "clear history  succeed";
            char *str = new char[strlen(sendMessage.c_str())];
            strcpy(str, sendMessage.c_str());
            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            continue;
        }
        if (r.arg[0] == "size")
        {
            //符合命令格式
            //获得键值
            string key = "";
            string value = "";
            bool flag = true;
            mymap::iterator it;
            it = server->requests.find("size");
            RequestFun cmd = it->second;
            void *tmp = cmd(server, c, key, value, flag);
            // 向客户端发送数据
            string sendMessage = std::to_string(*(int *)tmp);
            delete (int *)tmp;
            char *str = new char[strlen(sendMessage.c_str())];
            strcpy(str, sendMessage.c_str());
            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            continue;
        }
        if (r.arg[0] == "getfile")
        {
            //符合命令格式
            //获得键值
            string key = "";
            string value = "";
            bool flag = true;
            mymap::iterator it;
            it = server->requests.find("getfile");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            // 向客户端发送数据
            char *str = new char[strlen(value.c_str())];
            strcpy(str, value.c_str());
            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            continue;
        }
        if (r.arg[0] == "sitefile")
        {
            //符合命令格式
            //获得键值
            string key = "";
            string value = r.arg[1];
            int f = atoi(r.arg[2].c_str());
            bool flag;
            if (f > 0)
            {
                flag = true;
            }
            else
            {
                flag = false;
            }

            mymap::iterator it;
            it = server->requests.find("sitefile");
            RequestFun cmd = it->second;
            cmd(server, c, key, value, flag);
            // 向客户端发送数据
            string sendMessage = "";
            if (flag)
            {
                sendMessage.append("--- dump succeed ---\n");
            }
            sendMessage.append("--- clear memory succeed ---\n--- clear history succeed ---");
            char *str = new char[strlen(sendMessage.c_str())];
            strcpy(str, sendMessage.c_str());
            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            exitcount = 0;
            continue;
        }
        if (r.arg[0] == "exit")
        {
            if (exitcount == 0)
            {
                // 向客户端发送数据
                string sendMessage = "--- are you dump? ---";
                char *str = new char[strlen(sendMessage.c_str())];
                strcpy(str, sendMessage.c_str());
                write(c->sClient, str, strlen(str) + sizeof(char));
                delete[] str;
                ++exitcount;
                continue;
            }
            std::cout << "--- [pthread ID ]: " << c->tid << "  exit --- " << std::endl;
            string sendMessage = "exit";
            char *str = new char[strlen(sendMessage.c_str())];
            strcpy(str, sendMessage.c_str());
            write(c->sClient, str, strlen(str) + sizeof(char));
            delete[] str;
            break;
        }
    }
    sleep(1000);
    c->finished = false;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    short port = PORT;

    if (argc >= 2)
    {
        port = atoi(argv[1]);
    }

    //初始化服务端
    initServer(server, port);
    std::list<Client *> ls;
    for (int i = 0; i < MAXCLIENTNUM; ++i)
    {
        //等待客户端连接
        Client *client = new Client();
        socklen_t client_addr_len = sizeof(client->myClient);
        client->sClient = accept(server->sListen, (struct sockaddr *)&(client->myClient), &client_addr_len);
        if (client->sClient == -1)
        {
            perror("accept faild");
            continue;
        }
        ls.push_back(client);
        cout << "Accepted client:" << inet_ntoa((client->myClient).sin_addr) << " : "
             << ntohs((client->myClient).sin_port) << endl;

        //将客户端与服务端中对应的数据库连接,后续可能会针对此接口进行管理。
        server->DB[i] = &(client->db);

        //创建工作线程
        pthread_create(&client->tid, NULL, worker, client);
        pthread_detach(client->tid);
    }
    while (!ls.empty())
    {
        sleep(1000);
        Client *tmp = ls.front();
        ls.pop_front();
        if (!tmp->finished) //如果不需要了
        {
            delete tmp;
        }
        else
        { //还有用，还在跑
            ls.push_back(tmp);
        }
    }
    //关闭套接字
    close(server->sListen);
    delete server;
    cout << "ALL FINISHED" << endl;
    pthread_exit(NULL);
    return 0;
}
