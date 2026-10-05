#pragma once

#include "../headers/Nodes.hpp"
#include "../headers/lexer.hpp"

class Parser {
private:
  unsigned int index = 0;
  std::vector<Token> Input;

private:
  Token at();
  Token peek();

  Node *ParseExpr();
  Node *ParseFactor();
  Node *ParseTerm();

public:
  Node *Parse(std::vector<Token> raw);
};
