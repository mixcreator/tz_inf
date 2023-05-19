#!/bin/sh

rm ./tz
g++ -o ./tz main.cpp queue.cpp thread.cpp process.cpp raw_data.cpp -O3 -Wall -std=c++17 -Wpedantic -pedantic

# debug build
#g++ -o ./tz main.cpp queue.cpp thread.cpp process.cpp raw_data.cpp -O0 -g -Wall -std=c++17 -Wpedantic -pedantic
#ulimit -c unlimited
#./tz


#g++ -o ./tz main.cpp queue.cpp thread.cpp process.cpp -Wall -std=c++14 -Wpedantic -pedantic

#rm ./t_writer
#g++ -o t_writer writer.cpp -Wall -std=c++17 -Wpedantic -pedantic



#g++ -o test_queue main.cpp  -Wall -std=c++17 -Wpedantic -pedantic
