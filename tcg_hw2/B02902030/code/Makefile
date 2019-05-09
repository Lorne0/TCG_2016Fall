all:
	g++ search.cpp -std=c++11 -O2 -o B02902030

judge:
	g++ judge.cpp -std=c++11 -O2 -o judge

test:
	rm -rf log_ju.txt log_p1.txt log_p2.txt
	./judge 7122 > log_ju.txt &
	./B02902030 127.0.0.1 7122 > log_p1.txt &
	./B02902030 127.0.0.1 7122 > log_p2.txt &
