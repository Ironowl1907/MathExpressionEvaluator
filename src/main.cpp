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
  
  std::cout << "'Q' to quit" << '\n';
  while (1){
    std::cout << "calc >> ";
    std::string input; std::getline(std::cin,input);

    if (input == "q" || input == "Q" || input == "Quit" || input == "quit") return  0;

    std::vector<Token> TokenList = lexer.Tokenize(input);
    if (TokenList[0].type == Error) continue;
 
    std::cout << "N Tokens: " << TokenList.size() << '\n';

    std::cout << "------------------------" << '\n';

    Tree = parser.Parse(TokenList);
    if (Tree == nullptr) continue;
    std::cout << solve(Tree) << '\n';
  }

  delete Tree;    // Free the Tree memory
  return 0;
}
