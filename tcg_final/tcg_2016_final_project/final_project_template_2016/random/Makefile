CXX=g++
LDFLAGS=-static -s 
.PHONY:	clean
search:	main.cc anqi.cc ClientSocket.cpp Protocol.cpp
	$(CXX) -O2 -o $@ $^ $(LDFLAGS)
clean:
	DEL search.exe 2>NUL
