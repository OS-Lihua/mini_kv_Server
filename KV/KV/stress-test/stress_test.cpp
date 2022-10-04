/* ************************************************************************
> File Name:     stress_test.cpp
> Author:        Lihua
> Created Time:  2021.12.05
> Finish Time:   2021.12.07
> Reference:     https://github.com/youngyangyang04/Skiplist-CPP
 ************************************************************************/

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../skiplist.h"

#define NUM_THREADS 1      //线程数目
#define TEST_COUNT 1000000 //数据量

SkipList<int, std::string> skipList(18);

// 不断插入
void *insertElement(void *threadid)
{
    long tid;
    tid = (long)threadid;
    std::cout << tid << std::endl;
    int tmp = TEST_COUNT / NUM_THREADS;
    for (int i = tid * tmp, count = 0; count < tmp; i++)
    {
        count++;
        skipList.insert_element(rand() % TEST_COUNT, "a");
    }
    pthread_exit(NULL);
}

// 不断查询
void *getElement(void *threadid)
{
    long tid;
    tid = (long)threadid;
    std::cout << tid << std::endl;
    int tmp = TEST_COUNT / NUM_THREADS;
    for (int i = tid * tmp, count = 0; count < tmp; i++)
    {
        count++;
        skipList.search_element(rand() % TEST_COUNT);
    }
    pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));

    {

        pthread_t threads[NUM_THREADS];
        int rc;
        int i;

        auto start = std::chrono::high_resolution_clock::now();

        for (i = 0; i < NUM_THREADS; i++)
        {
            std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, insertElement, (void *)i);

            if (rc)
            {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for (i = 0; i < NUM_THREADS; i++)
        {
            if (pthread_join(threads[i], &ret) != 0)
            {
                perror("pthread_create() error");
                exit(3);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::nano> elapsed = finish - start;
        std::cout << "insert elapsed:" << elapsed.count() << "纳秒" << std::endl;
    }

#if 1

    skipList.display_list();

    {
        pthread_t threads[NUM_THREADS];
        int rc;
        int i;
        auto start = std::chrono::high_resolution_clock::now();

        for (i = 0; i < NUM_THREADS; i++)
        {
            std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, getElement, (void *)i);

            if (rc)
            {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for (i = 0; i < NUM_THREADS; i++)
        {
            if (pthread_join(threads[i], &ret) != 0)
            {
                perror("pthread_create() error");
                exit(3);
            }
        }

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::nano> elapsed = finish - start;
        std::cout << "get elapsed:" << elapsed.count() << "纳秒" << std::endl;
    }
#endif
    pthread_exit(NULL);
    return 0;
}
