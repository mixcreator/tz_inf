#!/bin/sh

export MallocStackLogging=1

rm ./tz_debug
g++ -O0 -g -v -w -o ./tz_debug main.cpp queue.cpp thread.cpp process.cpp raw_data.cpp -Wall -std=c++17

# Run leaks detecting tool
#leaks -atExit -- ./test_queue | grep LEAK:
leaks -atExit -- ./test_queue
