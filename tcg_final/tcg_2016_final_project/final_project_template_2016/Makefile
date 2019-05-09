CXX=g++
LDFLAGS=-static -s 
.PHONY:	clean
search:	main.cc anqi.cc ClientSocket.cpp Protocol.cpp
	$(CXX) -std=c++11 -O2 -o $@ $^ $(LDFLAGS)
clean:
	DEL search.exe 2>NUL
