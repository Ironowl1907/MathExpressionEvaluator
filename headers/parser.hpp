#pragma once

#include "../headers/lexer.hpp"
#include "../headers/Nodes.hpp"


class Parser
{
private:
  unsigned int index = 0;
  std::vector<Token> Input;

private:
  Token at();
  Token peak();

  Node* ParseExpr();
  Node* ParseFactor();
  Node* ParseTerm();

public:
  Node* Parse(std::vector<Token> raw);
};
