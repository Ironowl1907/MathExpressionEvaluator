final: main.o lexer.o
	g++ -o final main.o lexer.o

main.o: src/main.cpp
	g++ -c src/main.cpp

lexer.o: src/lexer.cpp
	g++ -c src/lexer.cpp
