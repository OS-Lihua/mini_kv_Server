/* ************************************************************************
> File Name:     main.cpp
> Author:        Lihua
> Created Time:  2021.12.05
> Finish Time:   2021.12.07
> Reference:     https://github.com/youngyangyang04/Skiplist-CPP
 ************************************************************************/

#include "skiplist.h"

int main()
{

    // 键值中的key用int型，如果用其他类型，需要自定义比较函数 operator <
    // 而且如果修改key的类型，同时需要修改skipList.load_file函数
    SkipList<int, std::string> skipList(6);
    skipList.insert_element(1, "skiplist");
    skipList.insert_element(3, "rbtree");
    skipList.insert_element(7, "hashmap");
    skipList.insert_element(8, "bitmap");
    skipList.insert_element(9, "queue");
    skipList.insert_element(19, "stack");
    skipList.insert_element(19, "btree");
    skipList.insert_element(13, "queue");
    skipList.insert_element(25, "list");
    skipList.insert_element(666, "lihua");

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.dump_file();

    // skipList.load_file();

    skipList.search_element(9);
    skipList.search_element(18);

    skipList.display_list();

    skipList.delete_element(3);
    skipList.delete_element(7);

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.display_list();
    return 0;
}
