#include <iostream>
#include <string>
#include <vector>

#include "../headers/lexer.hpp"
#include "../headers/parser.hpp"

#define log(x) std::cout << x << '\n'


int main (int argc, char *argv[]) {
  Lexer lexer;
  Parser parser;
  
  while (1){
    std::cout << "calc >> ";
    std::string input; std::getline(std::cin,input);
    std::vector<Token> TokenList = lexer.Tokenize(input);

    for (auto a : TokenList){
      std::cout << "Type: "<< a.type << " Value: " << a.value << '\n';
    }
    std::cout << "N Tokens: " << TokenList.size() << '\n';

    std::cout << "------------------------" << '\n';

    std::cout << parser.Parse(TokenList) << '\n';
  }
  return 0;
}
