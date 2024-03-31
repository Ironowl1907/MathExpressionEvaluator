#include <cctype>
#include <iostream>
#include <string>
#include <vector>

#include "../headers/lexer.hpp"

#define log(x) std::cout << x << '\n'

Token token (TokenType type, std::string value) { return {type, value}; };

char Lexer::at() { return RawInput[index]; };


std::vector<Token> Lexer::Tokenize(std::string Input){
  log(Input);

  RawInput = Input;
  Tokens = {};
  index = 0;
  while (index < RawInput.size()) {
    switch (at()) {
      case '\n':
      case '\t':
      case ' ':
        break;
      case '+':
        Tokens.push_back(token(Sum, "+"));
        break;
      case '-':
        Tokens.push_back(token(Rest, "-"));
        break;
      case '*':
        Tokens.push_back(token(Mul, "*"));
        break;
      case '/':
        Tokens.push_back(token(Div, "/"));
        break;
      case '(':
        Tokens.push_back(token(OpenPar, "("));
        break;
      case ')':
        Tokens.push_back(token(ClosePar, ")"));
        break;

      default:
      // Checks For numeric value
      if (isdigit(at())){
          std::string Num;
        while (isdigit(at())) {
          Num += at();
          Tokens.push_back(token(Integer,Num));
          index ++;
        }
          index --;
      }
      else { 
        std::cout << "Expected valid character, recived " << at() << " In position " << index << '\n';
        return {token(Error)};
      }
      
    }
    index ++;
  }
  return Lexer::Tokens;
}

