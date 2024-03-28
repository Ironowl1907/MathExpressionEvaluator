#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include "../headers/lexer.hpp"

#define log(x) std::cout << x << '\n'

Token token (TokenType type, int value) { return {type, value}; };

char Lexer::at() { return Lexer::RawInput[index]; };


std::vector<Token> Lexer::Tokenize(std::string Input){


  RawInput = Input;
  Tokens = {};
  index = 0;
  while (Lexer::index < Lexer::RawInput.size()) {
    switch (Lexer::at()) {
      case '\n':
      case '\t':
      case ' ':
        break;
      case '+':
        Lexer::Tokens.push_back(token(Sum));
        break;
      case '-':
        Lexer::Tokens.push_back(token(Rest));
        break;
      case '*':
        Lexer::Tokens.push_back(token(Mul));
        break;
      case '/':
        Lexer::Tokens.push_back(token(Div));
        break;
      case '(':
        Lexer::Tokens.push_back(token(OpenPar));
        break;
      case ')':
        Lexer::Tokens.push_back(token(ClosePar));

      default:
      // Checks For numeric value
      if (isdigit(Lexer::at())){
          std::string Num;
        while (isdigit(Lexer::at())) {
          Num += Lexer::at();
          Lexer::Tokens.push_back(token(Integer,stoi(Num)));
          Lexer::index ++;
        }
      }
      else { 
        std::cout << "Expected valid character, recived " << Lexer::at() << " In position " << Lexer::index << '\n';
        return {token(Error)};
      }
      
    }
    index ++;
  }
  return Lexer::Tokens;
}

