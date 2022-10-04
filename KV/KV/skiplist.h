/* ************************************************************************
> File Name:     skiplist.h
> Author:        Lihua
> Created Time:  2021.12.05
> Finish Time:   2021.12.07
> Reference:     https://github.com/youngyangyang04/Skiplist-CPP
 ************************************************************************/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <list>

const std::string STORE = "./store/";

std::mutex mtx; // 互斥锁
std::string delimiter = ":";

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

template <typename K, typename V>
Node<K, V>::Node(K k, V v, int level)
{
    this->key = k;
    this->value = v;
    this->node_level = level;

    // forword 指向下一级别的 一级指针
    // this->forward = (Node<K, V> *)calloc(sizeof(Node<K, V> *) * (level + 1));

    // forword 指向下一级别的 一级指针
    this->forward = new Node<K, V> *[level + 1];

    // 初始化为0
    memset(this->forward, 0, sizeof(Node<K, V> *) * (level + 1));
};

template <typename K, typename V>
Node<K, V>::~Node()
{
    delete[] forward;
};

template <typename K, typename V>
K Node<K, V>::get_key() const
{
    return key;
};

template <typename K, typename V>
V Node<K, V>::get_value() const
{
    return value;
};
template <typename K, typename V>
void Node<K, V>::set_value(V value)
{
    this->value = value;
};

///////////////////////////////////////////////////
// 跳表类
template <typename K, typename V>
class SkipList
{
public:
    SkipList(int max_level);
    ~SkipList();

    Node<K, V> *create_node(K, V, int); // 创建一个节点

    int get_random_level() const; //获取随机级别

    bool insert_element(const K key, const V value); // 插入元素
    bool search_element(const K key) const;          // 查找元素
    bool delete_element(const K key);                // 删除元素
    bool delete_revoke();                            // 撤销上一次删除
    void delete_completely();                        // 彻底删除

    void display_list() const; //展示跳表
    int size() const;          //获取元素个数

    void load_file(); // 下载文件       从磁盘加载数据到内存中
    void dump_file(); // 上传文件       将内存中的数据转储到磁盘文件中

    std::string get_file_name() const;          // 获取文件名
    void set_file_name(const std::string &str); //设置文件名

private:
    // 从字符串解析出 K V
    void get_key_value_from_string(const std::string &str, std::string *key, std::string *value);
    // 检测字符串是否有效
    bool is_valid_string(const std::string &str);

private:
    // 跳表最大级别  初始化时赋值
    int max_level;

    // 头节点
    Node<K, V> *header;

    // 被删除链表的头节点
    std::list<Node<K, V> *> delete_header;

    // 节点数目
    int element_count;

    // 跳表当前级别     元素存在的最大级别
    int skip_list_level;

    // 文件操作
    std::ifstream reader; // 读文件
    std::ofstream writer; // 写文件

    std::string store_file = "dumpFile";
};

template <typename K, typename V>
SkipList<K, V>::SkipList(int max_level)
{
    this->max_level = max_level;
    this->skip_list_level = 0;
    this->element_count = 0;

    // 创建一个K V 都是 null 的头节点
    K k;
    V v;
    this->header = new Node<K, V>(k, v, max_level);
}

template <typename K, typename V>
SkipList<K, V>::~SkipList()
{
    // 关闭 读写
    if (reader.is_open())
    {
        reader.close();
    }
    if (writer.is_open())
    {
        writer.close();
    }

    delete header;
}

// 创建一个节点
template <typename K, typename V>
Node<K, V> *SkipList<K, V>::create_node(const K k, const V v, int level)
{
    Node<K, V> *res = new Node<K, V>(k, v, level);
    return res;
}

// 获取随机级别
template <typename K, typename V>
int SkipList<K, V>::get_random_level() const
{

    int k = 1;
    while (rand() % 2)
    {
        ++k;
    }
    k = (k < max_level) ? k : max_level;
    return k;
};

// 获取元素个数
template <typename K, typename V>
int SkipList<K, V>::size() const
{
    return element_count;
}

// 从字符串解析出 K V
template <typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string &str, std::string *key, std::string *value)
{
    if (!is_valid_string(str))
    {
        return;
    }
    // 123:456
    *key = str.substr(0, str.find(delimiter));                  // 123
    *value = str.substr(str.find(delimiter) + 1, str.length()); // 456
}
// 检测字符串是否有效
template <typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string &str)
{
    if (str.empty()) //如果为空
    {
        return false;
    }

    if (str.find(delimiter) == std::string::npos) //如果没有 :
    {
        return false;
    }

    return true;
}

// 将内存中的数据转储到磁盘文件中
template <typename K, typename V>
void SkipList<K, V>::dump_file()
{

    std::cout << "------ dump  file  start ------" << std::endl;
    std::string File = STORE + store_file;
    writer.open(File); //打开

    Node<K, V> *node = this->header->forward[0];

    // 只存储 0 级别
    while (node != nullptr)
    {
        writer << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    writer.flush(); // 刷新缓冲区  如果前面输出使用 endl，则会自动刷新缓冲区
    writer.close(); // 关闭
    std::cout << "------- dump  file  end -------" << std::endl;
    return;
}

// 从磁盘加载数据到内存中，形成跳表
template <typename K, typename V>
void SkipList<K, V>::load_file()
{
    std::cout << "------ load  file  start ------" << std::endl;
    std::string File = STORE + store_file;
    reader.open(File);
    std::string line;
    std::string *key = new std::string();
    std::string *value = new std::string();
    while (getline(reader, line))
    {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty())
        {
            continue;
        }
        insert_element(*key, *value);
        std::cout << "------- load  file  end -------" << std::endl;
    }
    reader.close();
}

/* 关键算法 */
// 插入元素
/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10                  | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0   -oo   1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/
template <typename K, typename V>
bool SkipList<K, V>::insert_element(const K key, const V value)
{
    mtx.lock();
    std::cout << "----- insert element start. -----" << std::endl;
    bool res = false;
    Node<K, V> *current = this->header;

    // 创建更新数组并初始化它
    // update 是放置节点的数组，node->forward[i] 应该稍后操作
    // 在栈上开辟 指针数组
    Node<K, V> *update[max_level + 1];
    memset(update, 0, sizeof(Node<K, V> *) * (max_level + 1));

    // 从跳表的最高层开始
    // 创建到小于或等于自己节点的forward 数组
    // 示例: 若50 不存在 update -->{ 40,30,10, 1,1 }
    // 示例: 若50 存在   update -->{ 50,50,50,50,1 }
    for (int i = skip_list_level; i >= 0; --i)
    {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() <= key)
        {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // 如果当前节点的键等于搜索到的键，我们获取它
    if (current->get_key() == key)
    {
        std::cout << "key: " << key << " , exists, insert faild." << std::endl;
    }
    //     如果 current 的 key 不等于 key，这意味着我们必须在 (update[0] ,current] 之间插入节点
    else // if (current->get_key() != key)
    {

        // 为节点生成一个随机级别
        int random_level = get_random_level();

        // 如果(随机级别)新插入节点的级别 大于 跳表的当前级别， 更新前置指针
        if (random_level > skip_list_level)
        {
            for (int i = skip_list_level + 1; i < random_level + 1; ++i)
            {
                update[i] = header;
            }
            skip_list_level = random_level;
        }

        // 创建具有随机级别的新节点
        Node<K, V> *inserted_node = create_node(key, value, random_level);

        //插入节点
        for (int i = 0; i <= random_level; ++i)
        {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;

        ++element_count;
        res = true;
    }
    std::cout << "------ insert element end. ------" << std::endl;
    mtx.unlock();
    return true;
}
// 查找元素
template <typename K, typename V>
bool SkipList<K, V>::search_element(const K key) const
{
    std::cout << "----- search element start. -----" << std::endl;
    bool res = false;
    Node<K, V> *current = this->header;

    for (int i = skip_list_level; i >= 0; --i)
    {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() <= key)
        {
            current = current->forward[i];
        }
    }

    if (current->get_key() == key)
    {
        std::cout << "Found key: " << key << " , value: " << current->get_value() << std::endl;
        res = true;
    }
    else
    {
        std::cout << "Not Found Key:" << key << std::endl;
    }

    std::cout << "------ search element end. ------" << std::endl;
    return res;
}
// 删除元素
/*
                           +------------+
                           |  delete 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      delete +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10                  | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0   -oo   1    4  9  10         30   40  | 50 |  60      70       100
*/
template <typename K, typename V>
bool SkipList<K, V>::delete_element(const K key)
{
    mtx.lock();
    std::cout << "----- delete element start. -----" << std::endl;
    bool res = false;

    Node<K, V> *current = this->header; // 指向第一个节点
    // 创建更新数组并初始化它
    // update 是放置节点的数组，node->forward[i] 应该稍后操作
    // 在栈上开辟 指针数组
    Node<K, V> *update[max_level + 1];
    memset(update, 0, sizeof(Node<K, V> *) * (max_level + 1));

    // 创建到比自己小的节点的forward 数组 update -->{ 40,30 10 ,1,1 }
    for (int i = skip_list_level; i >= 0; --i)
    {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key)
        {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];
    if (current != nullptr && current->get_key() == key)
    {
        // 把前置指针归位
        for (int i = 0; i <= skip_list_level; ++i)
        {
            if (update[i]->forward[i] != current)
            {
                break;
            }

            update[i]->forward[i] = current->forward[i];
        }

        // 移除没有元素存在的级别
        while (skip_list_level > 0 && header->forward[skip_list_level] == nullptr)
        {
            --skip_list_level;
        }

        --element_count;
        std::cout << "Successfully deleted key: " << key << std::endl;

        // 将删除的元素添加到删除链表中
        delete_header.push_front(current);

        res = true;
    }
    else
    {
        std::cout << "Not Found Key: " << key << std::endl;
    }
    std::cout << "------ delete element end. ------" << std::endl;
    mtx.unlock();
    return res;
}

// 撤销上一次删除
template <typename K, typename V>
bool SkipList<K, V>::delete_revoke()
{
    bool res = false;
    if (delete_header.empty())
    {
        std::cout << "------ delete list is empty. ------" << std::endl;
    }
    else
    {
        insert_element(delete_header.front());
        delete_header.pop_front();
        std::cout << "------ revoket  Successful ------" << std::endl;
        res = true;
    }
    return res;
}

// 彻底删除
template <typename K, typename V>
void SkipList<K, V>::delete_completely()
{
    while (!delete_header.empty())
    {
        Node<K, V> *node = delete_header.front();
        delete node;
        delete_header.pop_front();
    }
}

// 展示跳表
template <typename K, typename V>
void SkipList<K, V>::display_list() const
{

    std::cout << "\n*****Skip List*****\n"
              << std::endl;
    for (int i = 0; i <= skip_list_level; i++)
    {
        Node<K, V> *node = this->header->forward[i];
        std::cout << "Level " << i << ": ";
        while (node != nullptr)
        {
            std::cout << node->get_key() << ":" << node->get_value() << ";  ";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

// 获取文件名
template <typename K, typename V>
std::string SkipList<K, V>::get_file_name() const
{
    return store_file;
}
//设置文件名
template <typename K, typename V>
void SkipList<K, V>::set_file_name(const std::string &str)
{
    store_file = str;
}
