#pragma once

#include "../headers/lexer.hpp"

class Parser
{
private:
  unsigned int index = 0;
  std::vector<Token> Input;

private:
  Token at();
  Token peak();

  int ParseExpr();
  int ParseFactor();
  int ParseTerm();
  int ParsePrefix();

public:
  int Parse(std::vector<Token> raw);
};
