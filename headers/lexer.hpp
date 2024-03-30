#pragma once
 
#include <vector>
#include <iostream>

enum TokenType {
  Integer,
  Sum,
  Rest,
  Div,
  Mul,
  OpenPar,
  ClosePar,
  Error
};


struct Token {
  TokenType type;
  std::string value = "Default";
};

Token token (TokenType type = Sum, std::string value = "Default");


class Lexer{
  private:
  std::vector<Token> Tokens;
  std::string RawInput;
  unsigned int index = 0;

  private:
  char at();

  public:
  std::vector<Token> Tokenize(std::string Input);
};
