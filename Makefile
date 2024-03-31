final: main.o lexer.o parser.o
	g++ -o final.out main.o lexer.o parser.o

main.o: src/main.cpp
	g++ -c -g src/main.cpp

lexer.o: src/lexer.cpp
	g++ -c -g src/lexer.cpp

parser.o: src/parser.cpp
	g++ -c -g src/parser.cpp

clear:
	rm *.o final.out
