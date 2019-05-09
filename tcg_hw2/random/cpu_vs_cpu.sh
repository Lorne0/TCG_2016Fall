g++ judge.cpp -std=c++11 -O2 -Wall -o judge
g++ search.cpp -std=c++11 -O2 -Wall -o p1
g++ search.cpp -std=c++11 -O2 -Wall -o p2
rm -rf log_ju.txt log_p1.txt log_p2.txt
./judge 7122 > log_ju.txt &
./p1 127.0.0.1 7122 > log_p1.txt &
./p2 127.0.0.1 7122 > log_p2.txt &

