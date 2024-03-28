final: main.o lexer.o parser.o
	g++ -o final main.o lexer.o parser.o

main.o: src/main.cpp
	g++ -c src/main.cpp

lexer.o: src/lexer.cpp
	g++ -c src/lexer.cpp

parser.o: src/parser.cpp
	g++ -c src/parser.cpp

clear:
	rm *.o final.out
