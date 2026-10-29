#include <iostream>
#include <string>
#include <vector>

#include "../headers/lexer.hpp"
#include "../headers/parser.hpp"

double solve(const Node *tree, const SymbolTable &symbols) {
  return tree->evaluate(symbols);
}

int main(int argc, char *argv[]) {
  Lexer lexer;
  Parser parser;
  SymbolTable symbolTable;

  std::cout << "'quit' to quit" << '\n';
  while (1) {
    std::cout << "calc >> ";
    std::string input;
    if (!std::getline(std::cin, input))
      break;

    if (input == "Quit" || input == "quit")
      return 0;

    std::vector<Token> TokenList = lexer.Tokenize(input);
    if (TokenList.empty() || TokenList[0].type == Error)
      continue;

    std::cout << "N Tokens: " << TokenList.size() << '\n';

    std::cout << "------------------------" << '\n';

    Statement stmt = parser.Parse(TokenList);
    if (stmt.node == nullptr)
      continue;

    try {
      double result = solve(stmt.node, symbolTable);
      if (stmt.isAssignment) {
        symbolTable[stmt.variable] = result;
      }
      std::cout << result << '\n';
    } catch (const EvaluationError &) {
    }

    delete stmt.node;
  }

  return 0;
}
