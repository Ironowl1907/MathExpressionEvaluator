#include <iostream>
#include <string>
#include <vector>

#include "../headers/lexer.hpp"
#include "../headers/parser.hpp"

#define log(x) std::cout << x << '\n'


int main (int argc, char *argv[]) {
  Lexer lexer;
  
  while (1){
    std::cout << "calc >> ";
    std::string input; std::getline(std::cin,input);
    std::vector<Token> TokenList = lexer.Tokenize(input);

    for (auto a : TokenList){
      std::cout << "Type: "<< a.type << " Value: " << a.value << '\n';
    }
  }
  return 0;
}
