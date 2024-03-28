final: main.o
	g++ main.o

main.o: src/main.cpp:
	g++ -c src/main.cpp
