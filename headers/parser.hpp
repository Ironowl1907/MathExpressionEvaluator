#pragma once

#include "../headers/Nodes.hpp"
#include "../headers/lexer.hpp"

struct Statement {
  bool isAssignment = false;
  char variable = '\0';
  Node *node = nullptr;
};

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
  Statement Parse(std::vector<Token> raw);
};
