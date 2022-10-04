#########################################################################
# File Name:     kvServer.sh
# Author:        Lihua
# Created Time:  2021.12.05 
# Finish Time:   2021.12.07
# Reference:     https://github.com/youngyangyang04/Skiplist-CPP 
#########################################################################
#!/bin/bash
g++ ./server/server.cpp -o ./bin/KvServer  --std=c++11 -pthread  
g++ ./client/client.cpp -o ./bin/KvClient  --std=c++11 -pthread  