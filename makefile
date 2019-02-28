a.out: main.cpp
	g++ -std=c++17 main.cpp -lpthread -O3

clean:
	rm -rf a.out

all: a.out


