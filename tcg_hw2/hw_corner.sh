g++ judge.cpp -std=c++11 -O2 -Wall -o judge
g++ search.cpp -std=c++11 -O2 -Wall -o b02902030
./judge 7122 100 > log_ju.txt &
./corner/p1 127.0.0.1 7122 > log_p1.txt &
./b02902030 127.0.0.1 7122 > log_p2.txt &

