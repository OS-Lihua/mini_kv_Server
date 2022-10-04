> File Name:     skiplist.h
> Author:        Lihua
> Created Time:  2021.12.05     9:03
> Finish Time:      2021.12.07	20:55
> Reference:     https://github.com/youngyangyang04/Skiplist-CPP

# KV存储引擎

众所周知，非关系型数据库redis，以及levedb，rockdb其核心存储引擎的数据结构就是跳表。

本项目就是基于跳表实现的轻量级键值型存储引擎，使用C++实现。插入数据、删除数据、查询数据、数据展示、数据落盘、文件加载数据，以及数据库大小显示。

在随机写读情况下，该项目每秒可处理啊请求数（QPS）: 24.39w，每秒可处理读请求数（QPS）: 18.41w

# 项目中文件

* main.cpp 包含skiplist.h使用跳表进行数据操作
* skiplist.h 跳表核心实现
* README.md 中文介绍       
* bin 生成可执行文件目录 
* makefile 编译脚本
* store 数据落盘的文件存放在这个文件夹 
* stress_test_start.sh 压力测试脚本
* LICENSE 使用协议

# 提供接口

* insert_element（插入数据）
* delete_element（删除数据）
* search_element（查询数据）
* display_list（展示已存数据）
* dump_file（数据落盘）
* load_file（加载数据）
* size（返回数据规模）
* delete_revoke   (撤销上一次删除)
* delete_completely (彻底删除)
* get_file_name  (获取文件名)
* set_file_name  (设置文件名)

# 存储引擎数据表现

## 插入操作

跳表树高：18 

采用随机插入数据测试：


|插入数据规模（万条） |耗时（秒） |
|---|---|
|10 |8.28415 |
|50 |37.4327 |
|100 |72.9611 |


每秒可处理写请求数（QPS）: 1.37w

## 取数据操作

|取数据规模（万条） |耗时（秒） |
|---|---|
|10|7.97065 |
|50|27.8785 |
|100|61.5887 |

每秒可处理读请求数（QPS）: 1.64w

# 项目运行方式

```
make            // complie demo main.cpp
./bin/main      // run 
```

如果想自己写程序使用这个kv存储引擎，只需要在你的CPP文件中include skiplist.h 就可以了。

可以运行如下脚本测试kv存储引擎的性能（当然你可以根据自己的需求进行修改）

```
sh stress_test_start.sh 
```

# 待优化 

* 压力测试并不是全自动的
* 跳表的key用int型，如果使用其他类型需要自定义比较函数，在K中重载 operator < 
* IO密集型的业务，如果将KV跳表的日志输入到日志文件，会比目前更快
* 如果再加上一致性协议，例如raft就构成了分布式存储，再启动一个http server就可以对外提供分布式存储服务了





