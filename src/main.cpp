#include <iostream>
#include <string>
#include <vector>

#include "../headers/lexer.hpp"
#include "../headers/parser.hpp"

double solve(const Node* tree){
  return tree->evaluate();
}

int main (int argc, char *argv[]) {
  Lexer lexer;
  Parser parser;
  Node* Tree;
  
  while (1){
    std::cout << "calc >> ";
    std::string input; std::getline(std::cin,input);
    std::vector<Token> TokenList = lexer.Tokenize(input);

    for (auto a : TokenList){
      std::cout << "Type: "<< a.type << " Value: " << a.value << '\n';
    }
    std::cout << "N Tokens: " << TokenList.size() << '\n';

    std::cout << "------------------------" << '\n';

    Tree = parser.Parse(TokenList);
    if (Tree == nullptr) continue;
    std::cout << solve(Tree) << '\n';
    break;
  }

  delete Tree;    // Free the Tree memory
  return 0;
}
