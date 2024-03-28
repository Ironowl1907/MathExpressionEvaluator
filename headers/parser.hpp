#pragma once

// #include <iostream>
// #include <vector>

#include "../headers/lexer.hpp"

// enum TokenType {
//   Integer,
//   Sum,
//   Rest,
//   Div,
//   Mul,
//   OpenPar, ClosePar,
//   Error
// };
//
// struct Token {
//   TokenType type;
//   int value = 0;
// };

class Parser {
  private:
  int index = 0;
  std::vector<Token> Input;

  private:
  Token at();

  int ParseExpr();
  int ParseFactor();
  int ParseTerm();

  public:
  int Parse(std::vector<Token> raw);
};

