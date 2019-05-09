g++ judge.cpp -std=c++11 -O2 -Wall -o judge
g++ search.cpp -std=c++11 -O2 -Wall -o b02902030
./judge 5478 100 > log_ju.txt &
./v2/p1 127.0.0.1 5478 > log_p1.txt &
./b02902030 127.0.0.1 5478 > log_p2.txt &

