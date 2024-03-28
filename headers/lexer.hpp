#pragma once

enum TokenType {
  Integer,
  Sum,
  Rest,
  Div,
  Mul,
  OpenPar, ClosePar,
  Error
};


struct Token {
  TokenType type;
  int value = 0;
};

Token token (TokenType type = Sum, int value = 0);


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
