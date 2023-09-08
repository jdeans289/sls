all: sls

sls: sls.cpp
	g++ sls.cpp -w -o sls -lncurses -std=c++11

clean:
	rm sls