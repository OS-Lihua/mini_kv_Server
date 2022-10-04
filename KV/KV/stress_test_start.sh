#########################################################################
# File Name:    stress_test_start.sh
# Author:        Lihua
# Created Time:  2021.12.05 
# Finish Time:   2021.12.07
# Reference:     https://github.com/youngyangyang04/Skiplist-CPP 
#########################################################################
#!/bin/bash
g++ stress-test/stress_test.cpp -o ./bin/stress  --std=c++11 -pthread  
./bin/stress
    