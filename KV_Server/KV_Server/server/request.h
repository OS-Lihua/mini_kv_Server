/* ************************************************************************
> File Name:     request.h
> Author:        Lihua
> Created Time:  2021.12.05
> Finish Time:   2021.12.07
> Reference:     https://github.com/youngyangyang04/Skiplist-CPP
 ************************************************************************/

#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <string>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::string;
#define orderSize 12
const string all_order[orderSize] = {"set", "get", "del", "load", "dump", "display", "revoke", "clear", "size", "getfile", "sitefile","exit"};
const int order_number = orderSize;
const string delimitero = " ";

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

// 解析请求
void Request::split_request()
{
    if (request.empty())
    {
        return;
    }
    string strs = get_request() + delimitero;

    size_t pos = strs.find(delimitero);
    size_t size = strs.size();

    // npos为结束位置
    while (pos != std::string::npos)
    {
        string x = strs.substr(0, pos);

        arg.push_back(x);
        strs = strs.substr(pos + 1, size);
        pos = strs.find(delimitero);
    }
}

// 判断请求是否有效
bool Request::is_valid_request()
{
    split_request();

    if (arg.size() == 0)
    {
        return false;
    }

    string order = arg[0];
    bool is_valid_order = false;
    for (int i = 0; i < order_number; ++i)
    {
        if (order == all_order[i])
        {
            is_valid_order = true;
            break;
        }
    }

    if (!is_valid_order)
    {
        print_error(1);
        return false;
    }

    if (order == all_order[0] && arg.size() != 3) // "set" set k v
    {
        print_error(2);
        return false;
    }

    if (order == all_order[1] && arg.size() != 2) // "get" get k
    {
        print_error(3);
        return false;
    }

    if (order == all_order[2] && arg.size() != 2) //"del" del k
    {
        print_error(4);
        return false;
    }
    if (order == all_order[3] && arg.size() != 1) //"load" load
    {
        print_error(5);
        return false;
    }

    if (order == all_order[4] && arg.size() != 1) // "dump" dump
    {
        print_error(6);
        return false;
    }
    if (order == all_order[5] && arg.size() != 1) // "display" display
    {
        print_error(7);
        return false;
    }
    if (order == all_order[6] && arg.size() != 1) // "revoke" revoke
    {
        print_error(8);
        return false;
    }
    if (order == all_order[7] && arg.size() != 1) // "clear" clear
    {
        print_error(9);
        return false;
    }
    if (order == all_order[8] && arg.size() != 1) // "size" size
    {
        print_error(10);
        return false;
    }
    if (order == all_order[9] && arg.size() != 1) // "getfile" getName
    {
        print_error(11);
        return false;
    }
    if (order == all_order[10] && arg.size() != 3) // "sitefile" setName kk ff
    {
        print_error(12);
        return false;
    }

    if (order == all_order[11] && arg.size() != 1) // "exit"  exit
    {
        print_error(0);
        return false;
    }

    return true;
}

// 输出错误日志
void Request::print_error(int error_number) const
{
    switch (error_number)
    {
    case 0:
        cout << "(error) ERR wrong number of arguments for 'exit' request" << endl;
        break;
    case 1:
        cout << "(error) ERR unknown request" << endl;
        break;
    case 2:
        cout << "(error) ERR wrong number of arguments for 'set' request" << endl;
        break;
    case 3:
        cout << "(error) ERR wrong number of arguments for 'get' request" << endl;
        break;
    case 4:
        cout << "(error) ERR wrong number of arguments for 'del' request" << endl;
        break;
    case 5:
        cout << "(error) ERR wrong number of arguments for 'load' request" << endl;
        break;
    case 6:
        cout << "(error) ERR wrong number of arguments for 'dump' request" << endl;
        break;
    case 7:
        cout << "(error) ERR wrong number of arguments for 'display' request" << endl;
        break;
    case 8:
        cout << "(error) ERR wrong number of arguments for 'revoke' request" << endl;
        break;
    case 9:
        cout << "(error) ERR wrong number of arguments for 'clear' request" << endl;
        break;
    case 10:
        cout << "(error) ERR wrong number of arguments for 'size' request" << endl;
        break;
    case 11:
        cout << "(error) ERR wrong number of arguments for 'getfile' request" << endl;
        break;
    case 12:
        cout << "(error) ERR wrong number of arguments for 'sitefile' request" << endl;
        break;

    default:
        break;
    }
}

#endif // ! __REQUEST_H__